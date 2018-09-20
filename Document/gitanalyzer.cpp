#include "gitanalyzer.h"
#include "apputils.h"

const QString GitAnalyzer::tagRev = "!R!v";
const QString GitAnalyzer::tagSHA = "!SHA:";
const QString GitAnalyzer::tagAuthor = "!AUT:";
const QString GitAnalyzer::tagNotes = "!NOTE:";

GitAnalyzer::GitAnalyzer(AnalyzeSettings settings, QObject *parent)
  : QObject(parent)
  , m_Settings(settings)
{
  m_Settings.CoreModulesNames = "core;uicore;OmUtils;OmUtilsUI";
      
  clearGenerationData();
}

void GitAnalyzer::clearGenerationData()
{
  m_gitProcessRunOK = true;
  m_gitProcessRunError = QProcess::UnknownError;
  m_CoreModulesRgxPattern.clear();
}

void GitAnalyzer::gitProcessFail(QProcess::ProcessError error)
{
  m_gitProcessRunOK = false;
  m_gitProcessRunError = error;
}

QString GitAnalyzer::makeGitProcessRunErrorString()
{
  QString errStr;
  switch( m_gitProcessRunError )
  {
    case QProcess::FailedToStart: errStr = "Невозможно запустить git.exe. Возможно не задан путь, поврежден или отсутствует файл."; break;
    case QProcess::Crashed: errStr = "Падение процесса git.exe."; break;
    case QProcess::Timedout: errStr = "Процесс git.exe не отвечает."; break;
    case QProcess::ReadError: errStr = "Невозможно прочесть данные процесса git.exe."; break;
    default: errStr = "Неизвестная ошибка при запуске git.exe."; break;
  }
  
  return errStr;
}

bool GitAnalyzer::getGitLog(QString &output, QString &errStr, LogFormat logFormat)
{
  QProcess gitProcess;
           gitProcess.setWorkingDirectory( m_Settings.RepositoryPath );

  QObject::connect(&gitProcess, &QProcess::errorOccurred, this, &GitAnalyzer::gitProcessFail);

  const QString argDateFormat = "dd.MM.yyyy";

  QStringList args;
              args.append("log");
              args.append( QString("--since=%1").arg( m_Settings.DateFrom.toString(argDateFormat) ) );
              args.append( QString("--until=%1").arg( m_Settings.DateTo.toString(argDateFormat) ) );
              

  switch( logFormat )
  {
    case LogFormat::FULL:
    args.append( QString("--pretty=format:\%1%2%H%3%an%4%s").arg(tagRev).arg(tagSHA).arg(tagAuthor).arg(tagNotes) );
    args.append( QString("--name-only") );
    break;
              
    case LogFormat::ONLY_REVTAGS:
    args.append( QString("--pretty=format:\%1").arg(tagRev) );
    break;
    
    default:
    errStr = "Получен неизвестный формат для лога git.";
    return false;
  }

  gitProcess.start("git", args);

  const bool runOK = gitProcess.waitForFinished();

  if( !runOK || !m_gitProcessRunOK )
  {
    errStr = makeGitProcessRunErrorString();

    return false;
  }

  QByteArray byteOutput = gitProcess.readAllStandardOutput();

  output = QString::fromUtf8(byteOutput);

  return true;
}

size_t GitAnalyzer::GetRevisionsCount(QString &errStr)
{
  clearGenerationData();

  QString gitOutput;

  if( !getGitLog( gitOutput, errStr, LogFormat::ONLY_REVTAGS ) )
    return 0;
  
  size_t revCount = 0;
  
  int findInd = 0;
  
  do
  {
    findInd = gitOutput.indexOf(tagRev, findInd);
    
    if( findInd != -1 )
    {
      revCount++;
      findInd += tagRev.size();
    }
  }
  while( findInd != -1 && findInd < gitOutput.size() );
    
  return revCount;
}

bool GitAnalyzer::ParseRevisionBody(const QString &revBodyStr, CRevisionData &revData)
{
  const int shaInd = revBodyStr.indexOf(tagSHA);
  const int authInd = revBodyStr.indexOf(tagAuthor,shaInd);
  const int notesInd = revBodyStr.indexOf(tagNotes,authInd);
  
  if( shaInd == -1 || authInd == -1 || notesInd == -1 )
  {
    Q_ASSERT( false ); // bad revision body
    return false;
  }
  
  const bool isMerge = revBodyStr.indexOf("Merge branch ",notesInd) != -1;
  
  if( !isMerge )
  {
    const bool changesCoreModules = revBodyStr.indexOf( QRegExp(coreModulesRgxPattern()), notesInd) != -1;
    
    revData.setChangesCore( changesCoreModules );
  }
  
  const int shaBodyInd = shaInd + tagSHA.size();
  const int authBodyInd = authInd + tagAuthor.size();
  const int notesBodyInd = notesInd + tagNotes.size();
  
  QString shaStr = revBodyStr.mid(shaBodyInd, authInd - (shaBodyInd));
  QString authStr = revBodyStr.mid(authBodyInd, notesInd - (authBodyInd));
  QString notesStr = revBodyStr.right( revBodyStr.size() - notesBodyInd );
  
  revData.setDeveloperName( authStr );
  revData.setSHA( shaStr );
  
  QRegExp taskTagRE;
          taskTagRE.setPattern("#(\\d)+(\\D|$)");
          
  const int taskTagPos = notesStr.indexOf( taskTagRE );
  revData.setRedmineLinked( taskTagPos != -1 );
  
  QRegExp hrsTagRE;
          hrsTagRE.setPattern("@\\d*[\\.,]*\\d*");
          
  notesStr.contains( hrsTagRE );
  
  QStringList hrsTagsList = hrsTagRE.capturedTexts();
  
  for( QString hrsTagStr : hrsTagsList )
  {
    QString hrsStr = hrsTagStr.right( hrsTagStr.size() - 1 );
            
    hrsStr.replace(',', '.');
    const double hrsVal = hrsStr.toDouble();
    revData.addHoursSpent( hrsVal );
  }
  
  return true;
}

QString GitAnalyzer::coreModulesRgxPattern()
{
  if( !m_CoreModulesRgxPattern.count() )
  {
    if( m_Settings.CoreModulesNames.count() )
    {
      QStringList modulesNames = m_Settings.CoreModulesNames.split(';', QString::SkipEmptyParts);
      
      m_CoreModulesRgxPattern = modulesNames.join("/|");
      m_CoreModulesRgxPattern += "/";
    }
  }
  
  return m_CoreModulesRgxPattern;
}

void GitAnalyzer::AddRevisionToDeveloper( std::vector<CDeveloperWorkData> &workDevList, const CRevisionData &revData )
{
  for(CDeveloperWorkData &devData : workDevList)
  {
    if( devData.getName().compare( revData.DeveloperName(), Qt::CaseInsensitive ) == 0 )
    {
      devData.addRevision( revData );
      break;
    }
  }
}

int GitAnalyzer::GetAnalyzeStepsCount()
{
  size_t revCount = GetRevisionsCount( QString() );
  return 2;
}

bool GitAnalyzer::AnalyzeRepository(std::vector<CDeveloperWorkData> &workDevList, QString *errStr)
{
  if( m_Settings.RepositoryPath.isEmpty() )
  {
    if( errStr )
      *errStr = "Не задан пусть к репозиторию Git";
    
    return false;
  }
  
  clearGenerationData();

  QString gitOutput;

  if( !getGitLog(gitOutput, errStr ? *errStr : QString(), LogFormat::FULL) )
    return false;
  
  emit analyzeStepDone();
  
  QStringList revisionsStrList = gitOutput.split(tagRev, QString::SkipEmptyParts);
  
  for( const QString &revBodyStr : revisionsStrList )
  {
    CRevisionData revData;
    if( ParseRevisionBody(revBodyStr, revData) )
    {
      AddRevisionToDeveloper( workDevList, revData );
    }
  }
  
  emit analyzeStepDone();
    
  return true;
}
