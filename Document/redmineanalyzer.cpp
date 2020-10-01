#include "redmineanalyzer.h"
#include "apputils.h"
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <memory>
#include <QSqlError>
#include <QSqlQuery>

RedmineAnalyzer::RedmineAnalyzer(AnalyzeSettings settings, QObject *parent) 
  : QObject(parent)
  , m_Settings(settings)
{
  while( m_Settings.Url.size() && m_Settings.Url.endsWith('/') )
    m_Settings.Url.chop(1);
}

bool RedmineAnalyzer::ParseTEData( QJsonDocument &jsonDoc, std::vector<CDeveloperWorkData> &workDevList, std::set<long> &issues, int &recordsRemain )
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
    
    issues.insert( issueID );
  }
  
  return true;
}

int RedmineAnalyzer::timeEntriesReadStepsCount() const
{
  return m_Settings.DateFrom.daysTo( m_Settings.DateTo ); // по шагу на каждый день  
}

int RedmineAnalyzer::trackersReadStepsCount() const
{
  return 1;
}

int RedmineAnalyzer::issuesReadStepsCount() const
{
  return timeEntriesReadStepsCount(); //предполагаем что она по длительности +/- равна timeEntries
}

int RedmineAnalyzer::GetAnalyzeStepsCount()
{
  return timeEntriesReadStepsCount() + trackersReadStepsCount() + issuesReadStepsCount();
}

bool RedmineAnalyzer::ReadTimeEntries( std::vector<CDeveloperWorkData> &workDevList, std::set<long> &issues, QString *errStr )
{
  for(QDate curDate = m_Settings.DateFrom; curDate <= m_Settings.DateTo; curDate = curDate.addDays(1))
  {
    const int recordsLimit = 100;
    int recordOffset = 0;
    int recordsRemain = 0;
    
    do
    {
      QJsonDocument jsonDoc;
      
      if( !ReadTEDataChunk( curDate, recordOffset, recordsLimit, jsonDoc ) )
      {
        if( errStr )
          *errStr = "Ошибка при обращении к redmine API.";
        
        return false;
      }
      
      if( !ParseTEData( jsonDoc, workDevList, issues, recordsRemain ) )
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

bool RedmineAnalyzer::AnalyzeServer(std::vector<CDeveloperWorkData> &workDevList, std::map<long,QString> &trackersList, std::map<long,CRedmineIssueData> &issues, QString *errStr)
{
  if( !CheckSettings( errStr ) )
    return false;

  if( !EstablishDBConnection( errStr ) )
    return false;
  
  std::set<long> involvedIssues;
  
  if( !ReadTimeEntries( workDevList, involvedIssues, errStr ) )
    return false;
  
  if( !ReadTrackers( trackersList, errStr ) )
    return false;
  
  std::set<long> overdueIssues;
  if( !ReadOverdueIssues( overdueIssues, errStr ) )
    return false;

  involvedIssues.insert( overdueIssues.begin(), overdueIssues.end() );

  if( !ReadIssues( involvedIssues, issues, errStr ) )
    return false;

  CloseDBConnection();
  
  return true;
}

void RedmineAnalyzer::CloseDBConnection()
{
  if( m_RMDB.isOpen() )
    m_RMDB.close();
}

bool RedmineAnalyzer::EstablishDBConnection( QString *errStr )
{
  if( m_RMDB.isOpen() )
    return true;

  m_RMDB = QSqlDatabase::addDatabase("QMYSQL", "OMPRM");
  m_RMDB.setHostName("192.168.222.48");
  m_RMDB.setPort(3306);
  m_RMDB.setDatabaseName("redmine");
  m_RMDB.setUserName("changes");
  m_RMDB.setPassword("changes");

  if( !m_RMDB.open() )
  {
    QSqlError err = m_RMDB.lastError();

    QString dstr = err.driverText();
    QString bstr = err.databaseText();

    if( errStr )
      *errStr = QString("Не удается подключиться к БД Redmine: %s").arg( dstr.length() ? dstr : bstr );

    return false;
  }

  return true;
}

bool RedmineAnalyzer::CheckSettings( QString *errStr )
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

    return true;
}

bool RedmineAnalyzer::ReadOverdueIssues( std::set<long> &issuesIds, QString *errStr  )
{
  Q_UNUSED(errStr);

  QString sqlStr = QString("SELECT cv.customized_id\
                           FROM issues i, custom_values cv\
                           WHERE\
                           i.status_id IN(%1) AND\
                           i.id = cv.customized_id AND\
                           cv.custom_field_id = %2 AND\
                           CHAR_LENGTH(cv.value) > 0 AND\
                           CONVERT(cv.value,DATE) < DATE(\"%3\")")
      .arg( valuesToString(CRedmineIssueData::openStates(), ",") )
      .arg( CRedmineIssueData::cvDEADLINE )
      .arg( QDate::currentDate().toString("yyyy-MM-dd") );

  QSqlQuery query(sqlStr,m_RMDB);

  while( query.next() )
  {
    issuesIds.insert( query.value(0).toInt() );
  }

  return true;
}

bool RedmineAnalyzer::ReadIssuesCloseDate( std::map<long,CRedmineIssueData> &issuesData, QString *errStr )
{
  Q_UNUSED(errStr);

  std::set<long> issuesCodes;
  MAP2SET1(issuesData,issuesCodes);

  QString sqlStr = QString("SELECT j.journalized_id, CONVERT(MAX(j.created_on),DATE)\
                           FROM journals j, journal_details jd\
                           WHERE\
                           jd.prop_key = 'status_id' AND\
                           jd.journal_id = j.id AND\
                           j.journalized_type = 'Issue' AND\
                           CONVERT(jd.old_value,UNSIGNED) IN(%1) AND\
                           CONVERT(jd.value,UNSIGNED) not IN(%1) AND\
                           j.journalized_id IN (%2)\
                           GROUP BY j.journalized_id")
      .arg( valuesToString(CRedmineIssueData::openStates(), ",") )
      .arg( valuesToString(issuesCodes, ",") );

  QSqlQuery query(sqlStr, m_RMDB);

  while( query.next() )
  {
    const int issueID = query.value(0).toInt();
    const QString dateStr = query.value(1).toString();
    const QDate closeDate = QDate::fromString( dateStr, "yyyy-MM-dd");

    issuesData[issueID].setCloseDate(closeDate);
  }

  return true;
}

bool RedmineAnalyzer::ReadIssues( const std::set<long> &issues, std::map<long,CRedmineIssueData> &issuesData, QString *errStr )
{
  issuesData.clear();
  
  if( issues.empty() )
  {
    for(int i = 0; i < issuesReadStepsCount(); i++)
      emit analyzeStepDone();
    
    return true;
  }
  
  std::vector<long> issuesList;
  
  SET2VEC(issues, issuesList);
  
  int stepsRemainsCount = issuesReadStepsCount();
  int issuesDone = 0;
  
  double progressPerIssue = (double)issuesReadStepsCount() / (double)issuesList.size();
  double curProgressSumm = 0.0;
  int progressDone = 0;
  
  const int maxRecordsPerStep = 20;
  
  for(int stepNum = 0; (stepNum * maxRecordsPerStep) <= (long)issuesList.size(); stepNum++)
  {
    int issuesOffset = stepNum * maxRecordsPerStep;
    
    std::vector<long> curIssuesList;
    curIssuesList.assign(issuesList.begin() + issuesOffset, issuesList.begin() + std::min( (stepNum+1) * maxRecordsPerStep, (int)issuesList.size() ) );
    
    const QString isssuesStr = valuesToString(curIssuesList, ",");
    
    QString chunckURL = QString("%1/issues.json/?key=%2;offset=%3;status_id=*;limit=%4;issue_id=%5")
        .arg(m_Settings.Url)
        .arg(m_Settings.AuthKey)
        .arg( 0 )
        .arg(maxRecordsPerStep)
        .arg( isssuesStr );
    
    QJsonDocument jsonDoc;
    
    if( !ReadJsonFromURL(chunckURL, jsonDoc) )
    {
      if( errStr )
        *errStr = "Ошибка при обращении к redmine API.";
      
      return false;
    }
    
    QJsonObject docObject = jsonDoc.object();
    
    QJsonArray issuesJsonArray = docObject.take("issues").toArray();
    
    for(int i = 0; i < issuesJsonArray.size(); i++)
    {
      CRedmineIssueData issueData;

      QJsonObject issueObject = issuesJsonArray.at(i).toObject();
      issueData.setID( issueObject.take("id").toInt() );
      
      QJsonObject trackerObject = issueObject.take("tracker").toObject();
      issueData.setTracker( trackerObject.take("id").toInt() );

      QJsonObject statusObject = issueObject.take("status").toObject();
      issueData.setState( statusObject.take("id").toInt() );

      QJsonObject performerObject = issueObject.take("assigned_to").toObject();
      issueData.setPerformerID( performerObject.take("id").toInt() );
      issueData.setPerformerName( performerObject.take("name").toString() );

      QVariant nameVar = issueObject.take("subject").toVariant();
      issueData.setName( nameVar.toString() );

      QJsonArray customFieldsArray = issueObject.take("custom_fields").toArray();
      for(int cfInd = 0; cfInd < customFieldsArray.size(); cfInd++)
      {
        QJsonObject fieldObject = customFieldsArray.at(cfInd).toObject();

        const int fieldID = fieldObject.take("id").toInt();

        if( fieldID == CRedmineIssueData::cvPlan )
        {
          issueData.setIsInPlan( fieldObject.take("value").toString().length() > 0 );
        }

        if( fieldID == CRedmineIssueData::cvDEADLINE )
        {
          const QDate dlDate = QDate::fromString(fieldObject.take("value").toString(), "yyyy-MM-dd");
          issueData.setDeadLine( dlDate );
        }
      }
      
      issuesData[issueData.ID()] = issueData;
      
      issuesDone++;
     
      curProgressSumm += progressPerIssue;
      for(int i = progressDone; i < (int)curProgressSumm; i++)
      {
        emit analyzeStepDone();
        stepsRemainsCount--;
      }
      progressDone = (int)curProgressSumm;
    }
  }
  
  if( !ReadIssuesCloseDate( issuesData, errStr ) )
    return false;
  
  while( stepsRemainsCount )
  {
    emit analyzeStepDone();
    stepsRemainsCount--;
  }
  
  return true;
}

bool RedmineAnalyzer::ReadTrackers( std::map<long,QString> &trackersList, QString *errStr )
{
  QString trackersURL = QString("%1/trackers.json/?key=%2")
      .arg(m_Settings.Url)
      .arg(m_Settings.AuthKey);
  
  QJsonDocument jsonDoc;
  
  if( !ReadJsonFromURL(trackersURL, jsonDoc) )
  {
    if( errStr )
      *errStr = "Ошибка при обращении к redmine API.";
    
    return false;
  }
  
  QJsonObject docObject = jsonDoc.object();
  
  QJsonArray trackersJsonArray = docObject.take("trackers").toArray();
  
  for(int i = 0; i < trackersJsonArray.count(); i++)
  {
    QJsonObject trackerObject = trackersJsonArray.at(i).toObject();
    
    const QString trackerName = trackerObject.take("name").toString();
    const int trackerID = trackerObject.take("id").toInt();
    
    trackersList[trackerID] = trackerName;
  }
  
  emit analyzeStepDone();
  
  return true;
}

bool RedmineAnalyzer::ReadJsonFromURL(QString url, QJsonDocument &jsonData)
{
  std::unique_ptr<QNetworkAccessManager> manager( new QNetworkAccessManager(this) );
  
  QNetworkReply *reply = manager->get( QNetworkRequest(QUrl(url)) );
  
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

bool RedmineAnalyzer::ReadTEDataChunk( QDate curDate, int recordOffset, int recordsLimit, QJsonDocument &jsonData )
{
  const QString requestDateStr = curDate.toString("yyyy-MM-dd");//2016-04-05
  
  QString chunckURL = QString("%1/time_entries.json/?key=%2;spent_on=%3;offset=%4;limit=%5")
      .arg(m_Settings.Url)
      .arg(m_Settings.AuthKey)
      .arg(requestDateStr)
      .arg(recordOffset)
      .arg(recordsLimit);
  
  return ReadJsonFromURL(chunckURL, jsonData);
}
