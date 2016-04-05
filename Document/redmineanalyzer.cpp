#include "redmineanalyzer.h"
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QJsonDocument>

RedmineAnalyzer::RedmineAnalyzer(AnalyzeSettings settings, QObject *parent) 
  : QObject(parent)
  , m_Settings(settings)
{
  while( m_Settings.Url.size() && m_Settings.Url.endsWith('/') )
    m_Settings.Url.chop(1);
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
  
  for(QDate curDate = m_Settings.DateFrom; curDate <= m_Settings.DateTo; curDate.addDays(1))
  {
    const int recordsLimit = 100;
    int recordOffset = 0;
    int recordsRead = 0;
    
    do
    {
      if( !ReadDataChunk( curDate, recordOffset, recordsLimit, recordsRead ) )
      {
        if( errStr )
          *errStr = "Ошибка при обращении к redmine API.";
        
        return false;
      }
      
      recordOffset += recordsRead;
    }
    while( recordsRead == recordsLimit );
  }
  
  return true;
}

bool RedmineAnalyzer::ReadDataChunk( QDate curDate, int recordOffset, int recordsLimit, int &recordsRead )
{
  const QString requestDateStr = curDate.toString("yyyy-MM-dd");//2016-04-05
  QString chunckURL = QString("%1/time_entries.json/?key=%2;spent_on=%3").arg(m_Settings.Url).arg(m_Settings.AuthKey).arg(requestDateStr);
  
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
    QJsonDocument jsonDoc = QJsonDocument::fromJson( chunkRawData, &jsonParseError );
    
    if( jsonParseError.error != QJsonParseError::NoError )
      return false;
  }
  else
  {
    return false;
  }
  
  reply->deleteLater();
}
