#ifndef GITANALYZER_H
#define GITANALYZER_H

#include "developerworkdata.h"

#include <QDate>
#include <QProcess>

class GitAnalyzer : public QObject
{
  Q_OBJECT
  
public:
  struct AnalyzeSettings
  {
    QString RepositoryPath;
    QDate DateFrom;
    QDate DateTo;
  };
  
public:
  explicit GitAnalyzer(AnalyzeSettings settings, QObject *parent = 0);
  
public:
  
  bool AnalyzeRepository(std::vector<CDeveloperWorkData> &workDevList, QString *errStr = nullptr);
  size_t GetRevisionsCount();

private:
  void clearGenerationData();
  
  QString makeGitProcessRunErrorString();

  bool runGit(QString &output, QString *errStr = nullptr);
  
  bool ParseRevisionBody(const QString &revBodyStr, CRevisionData &revData);
  void AddRevisionToDeveloper( std::vector<CDeveloperWorkData> &workDevList, const CRevisionData &revData );
  
private slots:
  void gitProcessFail(QProcess::ProcessError error);
  
private:
  AnalyzeSettings m_Settings;
  
  bool m_gitProcessRunOK;
  QProcess::ProcessError m_gitProcessRunError;

  static const QString tagRev;
  static const QString tagSHA;
  static const QString tagAuthor;
  static const QString tagNotes;
};

#endif // GITANALYZER_H
