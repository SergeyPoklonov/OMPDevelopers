#include "redmineanalyzer.h"
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

RedmineAnalyzer::RedmineAnalyzer(AnalyzeSettings settings, QObject *parent) 
  : QObject(parent)
  , m_Settings(settings)
{
  while( m_Settings.Url.size() && m_Settings.Url.endsWith('/') )
    m_Settings.Url.chop(1);
}

bool RedmineAnalyzer::ParseData( QJsonDocument &jsonDoc, std::vector<CDeveloperWorkData> &workDevList, int &recordsRemain )
{
  QJsonObject docObject = jsonDoc.object();
  
  const int offset = docObject.take("offset").toInt();
  const int limit = docObject.take("limit").toInt();
  const int totalCount = docObject.take("total_count").toInt();
  
  recordsRemain = totalCount - (offset + limit);
  
  QJsonArray teJsonArray = docObject.take("time_entries").toArray();
  
  for(int i = 0; i < teJsonArray.count(); i++)
  {
    QJsonObject entryObject = teJsonArray.at(i).toObject();
    
    QJsonObject authorObject = entryObject.take("user").toObject();
    QString authorStr = authorObject.take("name").toString();
    
    auto findedDevIt = std::find_if(workDevList.begin(), workDevList.end(), [authorStr](const CDeveloperWorkData &devData)->bool
    {
      return devData.getName() == authorStr;
    });
    
    if( findedDevIt == workDevList.end() )
      continue;
    
    CDeveloperWorkData &devData = *findedDevIt;
    
    QJsonObject issueObject = entryObject.take("issue").toObject();
    const int issueID = issueObject.take("id").toInt();
    
    const double hoursSpent = entryObject.take("hours").toDouble();
    
    QString shaStr;
    const QString gitRevTag("commit:omp_git|");
    const int shaLen = 40;
    QString commentStr = entryObject.take("comments").toString();
    const int revTagInd = commentStr.indexOf(gitRevTag);
    if( revTagInd != -1 )
      shaStr = commentStr.mid(revTagInd + gitRevTag.size(), shaLen);
    
    CRedmineTimeData redmineTimeData;
                     redmineTimeData.setDeveloperName( authorStr );
                     redmineTimeData.setHoursSpent(hoursSpent);
                     redmineTimeData.setIssueID( issueID );
                     redmineTimeData.setRevision( shaStr );
                     
    devData.addRedmineTime( redmineTimeData );
  }
  
  return true;
}

int RedmineAnalyzer::GetAnalyzeStepsCount()
{
  return m_Settings.DateFrom.daysTo( m_Settings.DateTo );
}

bool RedmineAnalyzer::AnalyzeServer(std::vector<CDeveloperWorkData> &workDevList, QString *errStr)
{
  if( m_Settings.AuthKey.isEmpty() )
  {
    if( errStr )
      *errStr = "Не задан redmine authentication key.";
    
    return false;
  }
  
  if( m_Settings.Url.isEmpty() )
  {
    if( errStr )
      *errStr = "Не задан redmine server URL.";
    
    return false;
  }
  
  for(QDate curDate = m_Settings.DateFrom; curDate <= m_Settings.DateTo; curDate = curDate.addDays(1))
  {
    const int recordsLimit = 100;
    int recordOffset = 0;
    int recordsRemain = 0;
    
    do
    {
      QJsonDocument jsonDoc;
      
      if( !ReadDataChunk( curDate, recordOffset, recordsLimit, jsonDoc ) )
      {
        if( errStr )
          *errStr = "Ошибка при обращении к redmine API.";
        
        return false;
      }
      
      if( !ParseData( jsonDoc, workDevList, recordsRemain ) )
      {
        if( errStr )
          *errStr = "Ошибка при разборе данных Redmine.";
        
        return false;
      }
      
      recordOffset += recordsLimit;
    }
    while( recordsRemain > 0 );
    
    emit analyzeStepDone();
  }
  
  return true;
}

bool RedmineAnalyzer::ReadDataChunk( QDate curDate, int recordOffset, int recordsLimit, QJsonDocument &jsonData )
{
  const QString requestDateStr = curDate.toString("yyyy-MM-dd");//2016-04-05
  
  QString chunckURL = QString("%1/time_entries.json/?key=%2;spent_on=%3;offset=%4;limit=%5")
      .arg(m_Settings.Url)
      .arg(m_Settings.AuthKey)
      .arg(requestDateStr)
      .arg(recordOffset)
      .arg(recordsLimit);
  
  QNetworkAccessManager *manager = new QNetworkAccessManager(this);
  
  QNetworkReply *reply = manager->get( QNetworkRequest(QUrl(chunckURL)) );
  
  QEventLoop loop;
  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  
  loop.exec(QEventLoop::ExcludeUserInputEvents);
  
  const QNetworkReply::NetworkError replyError = reply->error();
  
  if( replyError == QNetworkReply::NoError )
  {
    QByteArray chunkRawData = reply->readAll();
    
    QJsonParseError jsonParseError;
    jsonData = QJsonDocument::fromJson( chunkRawData, &jsonParseError );
    
    if( jsonParseError.error != QJsonParseError::NoError )
    {
      Q_ASSERT( false ); // network error
      return false;
    }
    
    if( jsonData.isNull() || !jsonData.isObject() )
    {
      Q_ASSERT( false ); // bad JSON in reply
      return false;
    }
  }
  else
  {
    return false;
  }
  
  reply->deleteLater();
  
  return true;
}
