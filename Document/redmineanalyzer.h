#ifndef REDMINEANALYZER_H
#define REDMINEANALYZER_H

#include "developerworkdata.h"

#include <QDate>
#include <QObject>
#include <QNetworkReply> 

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
  bool AnalyzeServer(std::vector<CDeveloperWorkData> &workDevList, QString *errStr = nullptr);
  
signals:
  
public slots:
  
private:
  bool ReadDataChunk( QDate curDate, int recordOffset, int recordsLimit, int &recordsRead );
  
private:
  AnalyzeSettings m_Settings;
};

#endif // REDMINEANALYZER_H
