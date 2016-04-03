#include "gitanalyzer.h"

GitAnalyzer::GitAnalyzer(QObject *parent)
  : QObject(parent)
{
  clear();
}

void GitAnalyzer::clear()
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

bool GitAnalyzer::AnalyzeRepository(AnalyzeSettings settings, std::vector<CDeveloperWorkData> &workDevList, QString *errStr)
{
  if( settings.RepositoryPath.isEmpty() )
  {
    if( errStr )
      *errStr = "Не задан пусть к репозиторию Git";
    
    return false;
  }
  
  clear();
  
  QProcess gitProcess;
  gitProcess.setWorkingDirectory( settings.RepositoryPath );
  
  QObject::connect(&gitProcess, &QProcess::errorOccurred, this, &GitAnalyzer::gitProcessFail);
  
  {
    QStringList args;
    args.append("log");
    args.append("--pretty=format:\"SHA:%H Author:%an%nNotes:%s%n\"");
    args.append("--since=30.03.2016");
    args.append("--until=01.04.2016");
    gitProcess.start("git", args);
  }
  const bool runOK = gitProcess.waitForFinished();
  
  if( !runOK || !m_gitProcessRunOK )
  {
    if( errStr )
      *errStr = makeGitProcessRunErrorString();
    
    return false;
  }
  
  /*if( runOK )
  {
    QByteArray byteOutput = gitProcess.readAllStandardOutput();

    QString str = QString::fromUtf8(byteOutput);
    QMessageBox::information(this, "Utf8",str);
  }*/
  
  return true;
}
