#ifndef REDMINEANALYZER_H
#define REDMINEANALYZER_H

#include "developerworkdata.h"

#include <QDate>
#include <QObject>
#include <QJsonDocument>

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
  bool ReadTimeEntries( std::vector<CDeveloperWorkData> &workDevList, QString *errStr );
  bool ReadTEDataChunk( QDate curDate, int recordOffset, int recordsLimit, QJsonDocument &jsonData );
  bool ParseTEData( QJsonDocument &jsonDoc, std::vector<CDeveloperWorkData> &workDevList, int &recordsRemain );
  
  bool ReadTrackers( std::map<long,QString> &trackersList, QString *errStr );
  
  bool ReadIssuesTrackers( std::map<long,long> &issuesToTrackers, QString *errStr );
  bool ReadTotalIssues(int &qty, QString *errStr );
  
  bool ReadJsonFromURL(QString url, QJsonDocument &jsonData);
  
private:
  AnalyzeSettings m_Settings;
  
  static const int m_IssuesStepsQty = 1000;
};

#endif // REDMINEANALYZER_H
