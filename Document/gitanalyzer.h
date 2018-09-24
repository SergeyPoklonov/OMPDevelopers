#ifndef GITANALYZER_H
#define GITANALYZER_H

#include "developerworkdata.h"

#include <QDate>
#include <QProcess>
#include <set>

class GitAnalyzer : public QObject
{
  Q_OBJECT
  
public:
  struct AnalyzeSettings
  {
    QString RepositoryPath;
    QDate DateFrom;
    QDate DateTo;
    QString CoreModulesNames;
    std::set<QString> ConsideredAuthors;
  };
  
public:
  explicit GitAnalyzer(AnalyzeSettings settings, QObject *parent = 0);
  
public:
  
  bool AnalyzeRepository(std::vector<CDeveloperWorkData> &workDevList, QString *errStr = nullptr);
  
  int  GetAnalyzeStepsCount();
  
  size_t GetRevisionsCount(QString &errStr);
  
signals:
  void analyzeStepDone();

private:
  void clearGenerationData();
  
  QString makeGitProcessRunErrorString();
  
  enum class LogFormat
  {
    FULL         = 1,
    ONLY_REVTAGS = 2
  };

  bool getGitLog(QString &output, QString &errStr, LogFormat f);
  
  bool ParseRevisionBody(const QString &revBodyStr, CRevisionData &revData);
  void AddRevisionToDeveloper( std::vector<CDeveloperWorkData> &workDevList, const CRevisionData &revData );
  
  QString coreModulesRgxPattern();
  
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
  
  QString m_CoreModulesRgxPattern;
};

#endif // GITANALYZER_H
