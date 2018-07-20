#include "devstatisticsdocument.h"

#include <Document/documentdatamanager.h>

#include <QFile>
#include <QTextStream>

DevStatisticsDocument::DevStatisticsDocument(QObject *parent) 
  : QObject(parent)
{
  m_GenerationDone = false;
  m_WorkingDaysQty = 0;
  m_LargeRevisionHrsMin = DefaultLargeRevHrsMin;
}

void DevStatisticsDocument::clear()
{
  m_DateFrom = m_DateTo = QDate::currentDate();
  m_WorkingDaysQty = 0;
  m_LargeRevisionHrsMin = DefaultLargeRevHrsMin;
  
  m_DevelopersWorkDataList.clear();
  
  m_GenerationDone = false;
  m_GenerationResultData.clear();
  
  m_GenerationStepNum = 0;
}

void DevStatisticsDocument::clearWorkingDevelopers()
{
  m_DevelopersWorkDataList.clear();
}

void DevStatisticsDocument::addWorkingDeveloper( CDeveloperData devData, unsigned holidaysDays )
{
  m_DevelopersWorkDataList.push_back( CDeveloperWorkData(devData, holidaysDays) );
}

void DevStatisticsDocument::setDateFrom(QDate d)
{
  m_DateFrom = d;
}

void DevStatisticsDocument::setDateTo(QDate d)
{
  m_DateTo = d;
}

void DevStatisticsDocument::setWorkingDaysQty(unsigned d)
{
  m_WorkingDaysQty = d;
}

double DevStatisticsDocument::getMinRevHrs() const
{
  return m_LargeRevisionHrsMin;
}

void DevStatisticsDocument::setMinRevHrs( double hrs )
{
  m_LargeRevisionHrsMin = hrs;
}

bool DevStatisticsDocument::checkSettings(QString &errStr)
{
  if( m_DevelopersWorkDataList.empty() )
  {
    errStr = "Не выбран ни один разработчик.";
    return false;
  }

  if( !m_DateFrom.isValid() )
  {
    errStr = "Не задано начало учитываемого периода.";
    return false;
  }

  if( !m_DateTo.isValid() )
  {
    errStr = "Не задан конец учитываемого периода.";
    return false;
  }

  if( m_DateFrom > m_DateTo )
  {
    errStr = "Невереный учитываемый период, дата начала больше даты окончания.";
    return false;
  }

  if( !m_WorkingDaysQty )
  {
    errStr = "Не задано количество рабочих дней в периоде.";
    return false;
  }

  const qint64 periodLen = m_DateFrom.daysTo( m_DateTo );
  if( qint64(m_WorkingDaysQty) > periodLen )
  {
    errStr = "Количество рабочих дней превышает количество дней в учитываемом периоде.";
    return false;
  }

  return true;
}

GeneralSettingsHolder& DevStatisticsDocument::generalSettings()
{
  DocumentDataManager *parentDoc = dynamic_cast<DocumentDataManager*>( parent() );
  
  Q_ASSERT( parentDoc );
  
  return parentDoc->generalSettings();
}

RedmineAnalyzer::AnalyzeSettings DevStatisticsDocument::redmineSettings()
{
  RedmineAnalyzer::AnalyzeSettings settings;
  
  settings.AuthKey = generalSettings().getRedmineAuthKey();
  settings.Url = generalSettings().getRedmineURL();
  settings.DateFrom = m_DateFrom;
  settings.DateTo = m_DateTo;
  
  return settings;
}

GitAnalyzer::AnalyzeSettings DevStatisticsDocument::gitSettings()
{
  GitAnalyzer::AnalyzeSettings gitSettings;
  gitSettings.RepositoryPath = generalSettings().getGitPath();
  gitSettings.DateFrom = m_DateFrom;
  gitSettings.DateTo = m_DateTo;  
                  
  return gitSettings;
}

bool DevStatisticsDocument::generateWorkData()
{
  m_GenerationDone = false;
  
  GitAnalyzer gitAnalyzer( gitSettings() );
  RedmineAnalyzer redmineAnalyzer( redmineSettings() );
  
  int totalStepsNum = gitAnalyzer.GetAnalyzeStepsCount() + redmineAnalyzer.GetAnalyzeStepsCount();
  
  m_GenerationStepNum = 0;
  
  emit generationStepsNumUpdated( totalStepsNum );
  
  QObject::connect(&gitAnalyzer, &GitAnalyzer::analyzeStepDone, this, &DevStatisticsDocument::childGenerationStepDone);
  QObject::connect(&redmineAnalyzer, &RedmineAnalyzer::analyzeStepDone, this, &DevStatisticsDocument::childGenerationStepDone);
  
  std::vector< CDeveloperWorkData > tempWorkersData = m_DevelopersWorkDataList;
  
  // git
  emit generationMessage( "Анализ репозитория git..." );
 
  QString errStr;
    
  if( !gitAnalyzer.AnalyzeRepository( tempWorkersData, &errStr ) )
  {
    emit generationErrorOccured( QString("ОШИБКА:%1").arg(errStr) );
    return false;
  }
  
  emit generationMessage( "Анализ репозитория git завершен." );
  
  // redmine
  emit generationMessage( "Анализ redmine..." );
  
  if( !redmineAnalyzer.AnalyzeServer( tempWorkersData, m_TrackersList, m_IssuesToTrackers, &errStr ) )
  {
    emit generationErrorOccured( QString("ОШИБКА:%1").arg(errStr) );
    return false;
  }
  
  emit generationMessage( "Анализ redmine завершен." );
  
  m_GenerationDone = true;
  m_GenerationResultData = tempWorkersData;
  
  return true;
}

void DevStatisticsDocument::childGenerationStepDone()
{
  emit generationStepsDone( ++m_GenerationStepNum );
}

bool DevStatisticsDocument::creatHTMLDataFile(QString filePath)
{
  QString htmlText;
  
  htmlText += "<!DOCTYPE HTML>\n";
  htmlText += "<html>\n";
  htmlText += "<head>\n";
  htmlText += "<meta charset=""utf-8"">\n";
  htmlText += QString("<title>Статистика по разработчикам %1-%2</title>\n").arg(m_DateFrom.toString("dd.MM.yyyy")).arg(m_DateTo.toString("dd.MM.yyyy"));
  htmlText += "</head>\n";
  htmlText += "<body>\n";
  htmlText += QString("<h2>Период: %1 - %2</h2>\n").arg(m_DateFrom.toString("dd.MM.yyyy")).arg(m_DateTo.toString("dd.MM.yyyy"));
  htmlText += QString("<p>Всего рабочего времени: <b>%1</b></p>\n").arg( m_WorkingDaysQty * WorkHrsInDay );
  
  std::sort(m_GenerationResultData.begin(), m_GenerationResultData.end(), [](const CDeveloperWorkData &f, const CDeveloperWorkData &s)
  {
    return f.getName() < s.getName();
  });
  
  for( const CDeveloperWorkData &devData : m_GenerationResultData )
  {
    if( devData.getWageRate() != 1.0 )
      htmlText += QString("<h3>%1 (%2)</h3>\n").arg( devData.getName() ).arg( devData.getWageRate() );
    else
      htmlText += QString("<h3>%1</h3>\n").arg( devData.getName() );
    
    const double redmineTotalHrs = devData.redmineTotalHrs();
    const double redmineDevelopHrs = devData.redmineDevelopHrs();
    const double redmineDevelopPrc = redmineTotalHrs != 0.0 ? (100 * redmineDevelopHrs / redmineTotalHrs) : 0.0;
    const double developmentOtherHrs = devData.developOtherHrs();
    
    std::map<int,double> trackersTime;
    std::vector<CRedmineTimeData> devTEList = devData.redmineTimesList();
    
    for( CRedmineTimeData &teData : devTEList )
    {
      const int trackerID = m_IssuesToTrackers[teData.IssueID()];
      trackersTime[trackerID] += teData.HoursSpent();
    }
    
    htmlText += QString("<p>Всего по Redmine,ч: %1").arg( redmineTotalHrs );
    
    if( redmineTotalHrs != 0.0 )
    {
      htmlText += QString("  Из них:");
      
      for( auto &trackerTime : trackersTime )
      {
        const double trackerHrs = trackerTime.second;
        
        if( m_TrackersList.find( trackerTime.first ) != m_TrackersList.end() )
        {
          const QString trackerName = m_TrackersList[trackerTime.first];
          const double trackerPrc = trackerHrs / (redmineTotalHrs / 100.0);
          
          htmlText += QString("  [%1] %2ч %3%").arg(trackerName).arg(trackerHrs).arg(trackerPrc);
        }
      }
    }
    
    htmlText += QString("</p>\n");
    
    htmlText += QString("<p>Разработка по Redmine,ч: %1</p>\n").arg( redmineDevelopHrs );
    htmlText += QString("<p>Разработка по Redmine,%: %1</p>\n").arg( redmineDevelopPrc );
    htmlText += QString("<p>Прочая разработка,ч: %1</p>\n").arg( developmentOtherHrs );
    
    std::vector<CRevisionData> nonRedmineRevisionsList = devData.nonRedmineRevisionsList(false);
    
    if( nonRedmineRevisionsList.size() )
    {
      htmlText += "<table border=1>\n";
      htmlText += QString("<caption>Ревизии без привязки к Redmine (%1)</caption>\n").arg( nonRedmineRevisionsList.size() );
      htmlText += "<tr>\n";
      htmlText += "<th>Трудоемкость</th>\n";
      htmlText += "<th>Ссылка</th>\n";
      htmlText += "</tr>\n";
      
      std::sort(nonRedmineRevisionsList.begin(), nonRedmineRevisionsList.end(), [](const CRevisionData &f, const CRevisionData &s)
      {
        return f.HoursSpent() < s.HoursSpent();
      });
      
      for( const CRevisionData &revData : nonRedmineRevisionsList )
      {
        htmlText += QString("<tr><td>%1</td><td>%2</td></tr>\n")
                      .arg( revData.HoursSpent() )
                      .arg( makeGitHTMLRevisionURL( revData.SHA() ) );
      }
      
      htmlText += "</table>\n";
    }
    
    std::vector<CRevisionData> largeRevisions = devData.revisionsList( m_LargeRevisionHrsMin );
        
    if( largeRevisions.size() )
    {
      htmlText += "<table border=1>\n";
      htmlText += QString("<caption>Большие ревизии (>=%1)</caption>\n").arg( m_LargeRevisionHrsMin );
      htmlText += "<tr>\n";
      htmlText += "<th>Трудоемкость</th>\n";
      htmlText += "<th>Ссылка</th>\n";
      htmlText += "<th>По Redmine</th>\n";
      htmlText += "</tr>\n";
      
      std::sort(largeRevisions.begin(), largeRevisions.end(), [](const CRevisionData &f, const CRevisionData &s)
      {
        return f.HoursSpent() < s.HoursSpent();
      });
      
      for( const CRevisionData &revData : largeRevisions )
      {
        htmlText += QString("<tr><td>%1</td><td>%2</td><td>%3</td> </tr>\n")
                      .arg( revData.HoursSpent() )
                      .arg( makeGitHTMLRevisionURL( revData.SHA() ) )
                      .arg( revData.RedmineLinked() ? "Да" : "Нет" );
      }
      
      htmlText += "</table>\n";
    }
    else
    {
      htmlText += QString("<p>Ревизии %1 ч и больше отсутствуют.</p>\n").arg( m_LargeRevisionHrsMin );
    }
    
    htmlText += QString("<p>Всего ревизий: %1</p>\n").arg( devData.revisionsCount() );
    
    const double totalWorkHours = redmineTotalHrs + developmentOtherHrs;
    
    htmlText += QString("<p>Всего,ч: %1</p>\n").arg( totalWorkHours );
    htmlText += QString("<p>Отпуск,дн: %1</p>\n").arg( devData.getHolidaysDays() );
    
    const double totalLabour = (totalWorkHours * (1/devData.getWageRate())) + (devData.getHolidaysDays() * WorkHrsInDay);
    
    htmlText += QString("<p>Всего с учетом ставки,ч: <b>%1</b></p>\n").arg( totalLabour );
  }
  
  htmlText += "</body>\n";
  htmlText += "</html>\n";
  
  QFile fileToSave(filePath);
  
  if( !fileToSave.open(QIODevice::WriteOnly | QIODevice::Text) )
    return false;

  QTextStream out(&fileToSave);
              out.setCodec( QLatin1String("UTF-8").data() );
              
  out << htmlText;
  
  return true;
}

QString DevStatisticsDocument::makeGitHTMLRevisionURL(const QString &sha)
{
  QString gitUrlPrefix = generalSettings().getGitWeb();
  
  while( gitUrlPrefix.size() && gitUrlPrefix.endsWith('/') )
    gitUrlPrefix.chop(1);
  
  QString htmlURLStr;
  
  if( gitUrlPrefix.size() )
  {
    QString fullUrlStr = QString("%1/commit/?r=omp.git&h=%2").arg( gitUrlPrefix ).arg( sha );
    
    htmlURLStr = QString("<a href=""%1"">%2</a>").arg( fullUrlStr ).arg( sha );
  }
  else
  {
    htmlURLStr = sha;
  }
  
  return htmlURLStr;
}

QString DevStatisticsDocument::getOutputHTMLDefaultFileName() const
{
  QString fileName = QString("OMPDevelopersStatistic_%1_%2").arg( m_DateFrom.toString("yyyy") ).arg( m_DateFrom.toString("MM") );
  if( m_DateFrom.month() != m_DateTo.month() )
    fileName += m_DateTo.toString("MM");
  fileName += ".html";
  
  return fileName;
}
