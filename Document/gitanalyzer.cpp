#include "gitanalyzer.h"

const QString GitAnalyzer::tagRev = "!R!V";
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
              args.append( QString("--pretty=format:\"%1%n%2%H%n%3%an%n%4%s%n\"").arg(tagRev).arg(tagSHA).arg(tagAuthor).arg(tagNotes) );
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
  
  int seekInd = 0;
  
  /*do
  {
    QString revBodyStr;
    PickNextRevisionBody(seekInd, gitOutput, revBodyStr);
    
    seekInd
    
  }while();*/
    
  return true;
}
