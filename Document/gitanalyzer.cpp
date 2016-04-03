#include "gitanalyzer.h"

#include <QProcess>

GitAnalyzer::GitAnalyzer()
{
  
}

bool GitAnalyzer::AnalyzeRepository(GitAnalyzer::AnalyzeSettings settings, std::vector<CDeveloperWorkData> &workDevList, QString *errStr)
{
  if( settings.RepositoryPath.isEmpty() )
  {
    if( errStr )
      *errStr = "Не задан пусть к репозиторию Git";
    
    return false;
  }
  
  QProcess gitProcess;
  gitProcess.setWorkingDirectory( settings.RepositoryPath );
  {
    QStringList args;
    args.append("log");
    args.append("--pretty=format:\"SHA:%H Author:%an%nNotes:%s%n\"");
    args.append("--since=30.03.2016");
    args.append("--until=01.04.2016");
    gitProcess.start("git", args);
  }
  const bool runOK = gitProcess.waitForFinished();
  
  /*if( runOK )
  {
    QByteArray byteOutput = gitProcess.readAllStandardOutput();

    QString str = QString::fromUtf8(byteOutput);
    QMessageBox::information(this, "Utf8",str);
  }*/
  
  return true;
}
