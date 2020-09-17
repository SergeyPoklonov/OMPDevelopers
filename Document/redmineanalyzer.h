#ifndef REDMINEANALYZER_H
#define REDMINEANALYZER_H

#include "developerworkdata.h"

#include <QDate>
#include <QObject>
#include <QJsonDocument>
#include <QSqlDatabase>
#include <set>

class QNetworkReply;

class RedmineAnalyzer : public QObject
{
  Q_OBJECT
  
public:
  struct AnalyzeSettings
  {
    QString Url;
    QString AuthKey;
    QDate DateFrom;
    QDate DateTo;
  };
  
public:
  explicit RedmineAnalyzer(AnalyzeSettings settings, QObject *parent = 0);
  
public:
  bool AnalyzeServer(std::vector<CDeveloperWorkData> &workDevList, std::map<long,QString> &trackersList, std::map<long,CRedmineIssueData> &issues, QString *errStr = nullptr);
  int  GetAnalyzeStepsCount();
  
signals:
  void analyzeStepDone();
  
public slots:
  
private:
  bool ReadTimeEntries( std::vector<CDeveloperWorkData> &workDevList, std::set<long> &issues, QString *errStr );
  bool ReadTEDataChunk( QDate curDate, int recordOffset, int recordsLimit, QJsonDocument &jsonData );
  bool ParseTEData( QJsonDocument &jsonDoc, std::vector<CDeveloperWorkData> &workDevList, std::set<long> &issues, int &recordsRemain );
  
  bool ReadTrackers( std::map<long,QString> &trackersList, QString *errStr );
  
  bool ReadOverdueIssues( std::set<long> &issuesIds, QString *errStr  );

  bool ReadIssues( const std::set<long> &issues, std::map<long,CRedmineIssueData> &issuesData, QString *errStr );

  bool ReadIssuesCloseDate( std::map<long,CRedmineIssueData> &issuesData, QString *errStr );
  
  bool ReadJsonFromURL(QString url, QJsonDocument &jsonData);

  bool CheckSettings( QString *errStr );

  bool EstablishDBConnection( QString *errStr );
  void CloseDBConnection();
  
  int timeEntriesReadStepsCount() const;
  int trackersReadStepsCount() const;
  int issuesReadStepsCount() const;
  
private:
  AnalyzeSettings m_Settings;

  QSqlDatabase m_RMDB;
};

#endif // REDMINEANALYZER_H
