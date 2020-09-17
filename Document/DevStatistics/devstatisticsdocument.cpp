#include "devstatisticsdocument.h"

#include <Document/documentdatamanager.h>

#include <QFile>
#include <QTextStream>

#include <Document/HTMLGenerator/HTMLGenerator.h>

static const QString g_DefCoreModules = "core;uicore;OmUtils;OmUtilsUI";

DevStatisticsDocument::DevStatisticsDocument(QObject *parent) 
  : QObject(parent)
  , m_RedmineEnabled(true)
  , m_LargeRevListEnabled(false)
  , m_CoreRevListEnabled(false)
  , m_CoreModulesNames(g_DefCoreModules)
{
  m_GenerationDone = false;
  m_LargeRevisionHrsMin = DefaultLargeRevHrsMin;
}

void DevStatisticsDocument::clear()
{
  m_DateFrom = m_DateTo = QDate::currentDate();
  m_RedmineEnabled = true;
  m_LargeRevListEnabled = false;
  m_CoreRevListEnabled = false;
  m_CoreModulesNames = g_DefCoreModules;
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

void DevStatisticsDocument::addWorkingDeveloper( CDeveloperData devData )
{
  m_DevelopersWorkDataList.push_back( CDeveloperWorkData(devData) );
}

QDate DevStatisticsDocument::getDateFrom() const
{
  return m_DateFrom;
}

QDate DevStatisticsDocument::getDateTo() const
{
  return m_DateTo;
}

void DevStatisticsDocument::setDateFrom(QDate d)
{
  m_DateFrom = d;
}

void DevStatisticsDocument::setDateTo(QDate d)
{
  m_DateTo = d;
}

std::map<long,CRedmineIssueData> DevStatisticsDocument::getIssues() const
{
  return m_Issues;
}

std::vector< CDeveloperWorkData > DevStatisticsDocument::getDevelopersStatisticData() const
{
  return m_GenerationResultData;
}

unsigned DevStatisticsDocument::getWorkingDaysQty() const
{
  return m_CommonCalendar.getWorkDaysQty(getDateFrom(), getDateTo());
}

bool DevStatisticsDocument::isRedmineEnabled() const
{
  return m_RedmineEnabled;
}

void DevStatisticsDocument::setRedmineEnabled(bool isEnabled)
{
  m_RedmineEnabled = isEnabled;
}

bool DevStatisticsDocument::isLargeRevisionListEnabled() const
{
  return m_LargeRevListEnabled;
}

void DevStatisticsDocument::setLargeRevisionListEnabled(bool isEnable)
{
  m_LargeRevListEnabled = isEnable;
}

bool DevStatisticsDocument::isCoreRevisionListEnabled() const
{
  return m_CoreRevListEnabled;
}

void DevStatisticsDocument::setCoreRevisionListEnabled(bool isEnable)
{
  m_CoreRevListEnabled = isEnable;
}

QString DevStatisticsDocument::getCoremodulesNames() const
{
  return m_CoreModulesNames;
}

void DevStatisticsDocument::setCoreModulesNames(const QString &names)
{
  m_CoreModulesNames = names;
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

  if( !getWorkingDaysQty() )
  {
    errStr = "В указанном периоде отсутствуют рабочие дни.";
    return false;
  }
  
  if( isCoreRevisionListEnabled() && !getCoremodulesNames().count() )
  {
    errStr = "Не введены базовые модули.";
    return false;
  }

  return true;
}

DocumentDataManager& DevStatisticsDocument::parentDocument()
{
  DocumentDataManager *parentDoc = dynamic_cast<DocumentDataManager*>( parent() );
  
  Q_ASSERT( parentDoc );
  
  return *parentDoc;
}

GeneralSettingsHolder& DevStatisticsDocument::generalSettings()
{
  return parentDocument().generalSettings();
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
  
  for(const auto &dev : m_DevelopersWorkDataList)
    gitSettings.ConsideredAuthors.insert( dev.getName() );
                  
  return gitSettings;
}

bool DevStatisticsDocument::generateWorkData()
{
  m_GenerationDone = false;
  
  GitAnalyzer gitAnalyzer( gitSettings() );
  
  RedmineAnalyzer redmineAnalyzer( redmineSettings() );
  
  int totalStepsNum = gitAnalyzer.GetAnalyzeStepsCount();
  
  if( isRedmineEnabled() )
    totalStepsNum += redmineAnalyzer.GetAnalyzeStepsCount();
  
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
  if( isRedmineEnabled() )
  {
    emit generationMessage( "Анализ redmine..." );
    
    if( !redmineAnalyzer.AnalyzeServer( tempWorkersData, m_TrackersList, m_Issues, &errStr ) )
    {
      emit generationErrorOccured( QString("ОШИБКА:%1").arg(errStr) );
      return false;
    }
    
    emit generationMessage( "Анализ redmine завершен." );
  }
  
  m_GenerationDone = true;
  m_GenerationResultData = tempWorkersData;
  
  return true;
}

void DevStatisticsDocument::childGenerationStepDone()
{
  emit generationStepsDone( ++m_GenerationStepNum );
}

bool DevStatisticsDocument::creatHTMLDataFiles(QString dirPath)
{
  auto htmlFileNamesAndTexts = HTMLGenerator().generateHTMLFilesTexts(parentDocument());

  for(auto& fileNameAndText : htmlFileNamesAndTexts)
  {
    QFile fileToSave(dirPath + "\\" + fileNameAndText.first);

    if( !fileToSave.open(QIODevice::WriteOnly | QIODevice::Text) )
      return false;

    QTextStream out(&fileToSave);
                out.setCodec( QLatin1String("UTF-8").data() );

    out << fileNameAndText.second;
  }

  return true;
}

QString DevStatisticsDocument::getOutputHTMLDefaultFileName(bool withExtension) const
{
  QString fileName = QString("OMPDevelopersStatistic_%1_%2").arg( m_DateFrom.toString("yyyy") ).arg( m_DateFrom.toString("MM") );
  if( m_DateFrom.month() != m_DateTo.month() )
    fileName += m_DateTo.toString("MM");

  if( withExtension )
    fileName += ".html";
  
  return fileName;
}

QString DevStatisticsDocument::trackerName(int trackerID) const
{
  auto findedItr = m_TrackersList.find(trackerID);
  return findedItr != m_TrackersList.end() ? findedItr->second : "";
}

int DevStatisticsDocument::issueToTracker(int issueID) const
{
  auto findedItr = m_Issues.find(issueID);
  return findedItr != m_Issues.end() ? findedItr->second.Tracker() : 0;
}
