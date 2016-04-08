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
  clearGenerationData();
}

void GitAnalyzer::clearGenerationData()
{
  m_gitProcessRunOK = true;
  m_gitProcessRunError = QProcess::UnknownError;
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

bool GitAnalyzer::runGit(QString &output, QString *errStr)
{
  QProcess gitProcess;
           gitProcess.setWorkingDirectory( m_Settings.RepositoryPath );

  QObject::connect(&gitProcess, &QProcess::errorOccurred, this, &GitAnalyzer::gitProcessFail);

  const QString argDateFormat = "dd.MM.yyyy";

  QStringList args;
              args.append("log");
              args.append( QString("--pretty=format:\%1%2%H%3%an%4%s").arg(tagRev).arg(tagSHA).arg(tagAuthor).arg(tagNotes) );
              args.append( QString("--since=%1").arg( m_Settings.DateFrom.toString(argDateFormat) ) );
              args.append( QString("--until=%1").arg( m_Settings.DateTo.toString(argDateFormat) ) );

  gitProcess.start("git", args);

  const bool runOK = gitProcess.waitForFinished();

  if( !runOK || !m_gitProcessRunOK )
  {
    if( errStr )
      *errStr = makeGitProcessRunErrorString();

    return false;
  }

  QByteArray byteOutput = gitProcess.readAllStandardOutput();

  output = QString::fromUtf8(byteOutput);

  return true;
}

size_t GitAnalyzer::GetRevisionsCount()
{
  clearGenerationData();

  QString gitOutput;

  if( !runGit( gitOutput ) )
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

  if( !runGit(gitOutput, errStr) )
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
