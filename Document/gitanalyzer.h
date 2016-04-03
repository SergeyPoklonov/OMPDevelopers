#ifndef GITANALYZER_H
#define GITANALYZER_H

#include "developerworkdata.h"

#include <QDate>
#include <QProcess>

class AnalyzeSettings
{
public:
  QString RepositoryPath;
  QDate DateFrom;
  QDate DateTo;
};

class GitAnalyzer : public QObject
{
  Q_OBJECT
  
public:
  explicit GitAnalyzer(QObject *parent = 0);
  
public:
  
  
  bool AnalyzeRepository(AnalyzeSettings settings, std::vector<CDeveloperWorkData> &workDevList, QString *errStr = nullptr);
  
  
  
private:
  void clear();
  
  QString makeGitProcessRunErrorString();
  
private slots:
  void gitProcessFail(QProcess::ProcessError error);
  
private:
  bool m_gitProcessRunOK;
  QProcess::ProcessError m_gitProcessRunError;
};

#endif // GITANALYZER_H
