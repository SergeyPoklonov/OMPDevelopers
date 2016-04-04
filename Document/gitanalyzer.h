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
  explicit GitAnalyzer(AnalyzeSettings settings, QObject *parent = 0);
  
public:
  
  bool AnalyzeRepository(std::vector<CDeveloperWorkData> &workDevList, QString *errStr = nullptr);
  size_t GetRevisionsCount();

private:
  void clearGenerationOutData();
  
  QString makeGitProcessRunErrorString();

  bool runGit(QString &output, QString *errStr = nullptr);
  
private slots:
  void gitProcessFail(QProcess::ProcessError error);
  
private:
  AnalyzeSettings m_Settings;
  
  bool m_gitProcessRunOK;
  QProcess::ProcessError m_gitProcessRunError;

  static const QString tagSHA;
  static const QString tagAuthor;
  static const QString tagNotes;
};

#endif // GITANALYZER_H
