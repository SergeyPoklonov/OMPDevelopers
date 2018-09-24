#ifndef REDMINEANALYZER_H
#define REDMINEANALYZER_H

#include "developerworkdata.h"

#include <QDate>
#include <QObject>
#include <QJsonDocument>
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
  bool AnalyzeServer(std::vector<CDeveloperWorkData> &workDevList, std::map<long,QString> &trackersList, std::map<long,long> &issuesToTrackers, QString *errStr = nullptr);
  int  GetAnalyzeStepsCount();
  
signals:
  void analyzeStepDone();
  
public slots:
  
private:
  bool ReadTimeEntries( std::vector<CDeveloperWorkData> &workDevList, std::set<long> &issues, QString *errStr );
  bool ReadTEDataChunk( QDate curDate, int recordOffset, int recordsLimit, QJsonDocument &jsonData );
  bool ParseTEData( QJsonDocument &jsonDoc, std::vector<CDeveloperWorkData> &workDevList, std::set<long> &issues, int &recordsRemain );
  
  bool ReadTrackers( std::map<long,QString> &trackersList, QString *errStr );
  
  bool ReadIssuesTrackers( const std::set<long> &issues, std::map<long,long> &issuesToTrackers, QString *errStr );
  
  bool ReadJsonFromURL(QString url, QJsonDocument &jsonData);
  
  int timeEntriesReadStepsCount() const;
  int trackersReadStepsCount() const;
  int issuesReadStepsCount() const;
  
private:
  AnalyzeSettings m_Settings;
};

#endif // REDMINEANALYZER_H
