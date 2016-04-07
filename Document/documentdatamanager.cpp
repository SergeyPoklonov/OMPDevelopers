#include "developerdatamanager.h"
#include "documentdatamanager.h"
#include "gitanalyzer.h"
#include "redmineanalyzer.h"

#include <QCoreApplication>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>

DocumentDataManager::DocumentDataManager(QObject *parent) : QObject(parent)
{
  Construct();
}

CDeveloperListDataManager& DocumentDataManager::getDevelopersManager()
{
  return *m_DevelopersManager;
}

void DocumentDataManager::Construct()
{
  m_DevelopersManager = new CDeveloperListDataManager( this );
  m_GenerationDone = false;
  m_WorkingDaysQty = 0;
  m_LargeRevisionHrsMin = 3;
}

void DocumentDataManager::clear()
{
  m_DevelopersManager->clear();

  m_DateFrom = m_DateTo = QDate::currentDate();
  m_WorkingDaysQty = 0;
  m_LargeRevisionHrsMin = 3;
  
  m_GitRepositoryPath.clear();
  m_GitWebURL.clear();
  
  m_RedmineURL.clear();
  m_RedmineAuthKey.clear();
  
  m_DevelopersWorkDataList.clear();
  
  m_GenerationDone = false;
  m_GenerationResultData.clear();
  
  m_GenerationStepNum = 0;
}

void DocumentDataManager::SetGitPath(QString gitPath)
{
  m_GitRepositoryPath = gitPath;
  
  emit gitRepositoryChanged(m_GitRepositoryPath);
}

QString DocumentDataManager::GetGitPath() const
{
  return m_GitRepositoryPath;
}

QString DocumentDataManager::getOutputHTMLDefaultFilePath() const
{
  QString fileName = QString("OMPDevelopersStatistic_%1_%2").arg( m_DateFrom.toString("yyyy") ).arg( m_DateFrom.toString("MM") );
  if( m_DateFrom.month() != m_DateTo.month() )
    fileName += m_DateTo.toString("MM");
  fileName += ".html";
  
  QString dirParth = QCoreApplication::instance()->applicationDirPath();
  
  QString filePath = QString("%1/%2").arg( dirParth ).arg( fileName );
  
  return filePath;
}

QString DocumentDataManager::getGeneralSettingsFilePath() const
{
  QString dirParth = QCoreApplication::instance()->applicationDirPath();
  QString filePath = QString("%1/%2").arg( dirParth ).arg( "OMPDevSettings.xml" );
  return filePath;
}

bool DocumentDataManager::MakeGeneralSettingsXML(QString &xmlFileText)
{
  QDomDocument doc("OMPDevSettings");
  QDomElement root = doc.createElement("SettingsRoot");

  doc.appendChild(root);

  // developers
  m_DevelopersManager->WriteToXML( root );
  
  // git
  QDomElement gitElement = doc.createElement("Git");
  
  root.appendChild( gitElement );
  
  gitElement.setAttribute( "RepoPath", m_GitRepositoryPath );
  
  // redmine
  QDomElement redmineElement = doc.createElement("Redmine");
  
  root.appendChild( redmineElement );
  
  redmineElement.setAttribute( "URL", m_RedmineURL );
  redmineElement.setAttribute( "Key", m_RedmineAuthKey );

  xmlFileText = doc.toString();

  return true;
}

bool DocumentDataManager::LoadGeneralSettings()
{
  clear();
  
  QString filePath = getGeneralSettingsFilePath();

  QFile settingsFile( filePath );
  if( !settingsFile.open(QIODevice::ReadOnly) )
    return false;

  QDomDocument doc("OMPDevSettings");

  if( !doc.setContent(&settingsFile) )
  {
    settingsFile.close();
    return false;
  }

  QDomElement root = doc.documentElement();

  if( root.isNull() )
    return false;

  m_DevelopersManager->LoadFromXML( root );
  
  QDomElement gitElement = root.firstChildElement( "Git" );
  
  m_GitRepositoryPath = gitElement.attribute( "RepoPath" );
  
  emit gitRepositoryChanged(m_GitRepositoryPath);
  
  QDomElement redmineElement = root.firstChildElement( "Redmine" );
  
  m_RedmineURL = redmineElement.attribute("URL");
  m_RedmineAuthKey = redmineElement.attribute("Key");

  return true;
}

void DocumentDataManager::ClearWorkingDevelopers()
{
  m_DevelopersWorkDataList.clear();
}

RedmineAnalyzer::AnalyzeSettings DocumentDataManager::redmineSettings()
{
  RedmineAnalyzer::AnalyzeSettings settings;
  
  settings.AuthKey = m_RedmineAuthKey;
  settings.Url = m_RedmineURL;
  settings.DateFrom = m_DateFrom;
  settings.DateTo = m_DateTo;
  
  return settings;
}

GitAnalyzer::AnalyzeSettings DocumentDataManager::gitSettings()
{
  GitAnalyzer::AnalyzeSettings gitSettings;
  gitSettings.RepositoryPath = m_GitRepositoryPath;
  gitSettings.DateFrom = m_DateFrom;
  gitSettings.DateTo = m_DateTo;  
                  
  return gitSettings;
}

QString DocumentDataManager::getRedmineAuthKey() const
{
  return m_RedmineAuthKey;
}

void DocumentDataManager::setRedmineAuthKey(const QString &RedmineAuthKey)
{
  m_RedmineAuthKey = RedmineAuthKey;
}

QString DocumentDataManager::getRedmineURL() const
{
  return m_RedmineURL;
}

void DocumentDataManager::setRedmineURL(const QString &RedmineURL)
{
  m_RedmineURL = RedmineURL;
}

void DocumentDataManager::childGenerationStepDone()
{
  emit generationStepsDone( ++m_GenerationStepNum );
}

bool DocumentDataManager::creatHTMLDataFile(QString filePath)
{
  QString htmlText;
  
  htmlText += "<!DOCTYPE HTML>\n";
  htmlText += "<html>\n";
  htmlText += "<head>\n";
  htmlText += "<meta charset=""utf-8"">\n";
  htmlText += QString("<title>Статистика по разработчикам %1-%2</title>\n").arg(m_DateFrom.toString("dd.MM.yyyy")).arg(m_DateTo.toString("dd.MM.yyyy"));
  htmlText += "</head>\n";
  htmlText += "<body>\n";
  htmlText += QString("<p>Период: %1 - %2</p>\n").arg(m_DateFrom.toString("dd.MM.yyyy")).arg(m_DateTo.toString("dd.MM.yyyy"));
  htmlText += QString("<p>Всего рабочего времени: %1</p>\n").arg( m_WorkingDaysQty * WorkHrsInDay );
  
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
    
    htmlText += QString("<p>Всего по Redmine,ч: %1</p>\n").arg( redmineTotalHrs );
    htmlText += QString("<p>Разработка по Redmine,ч: %1</p>\n").arg( redmineDevelopHrs );
    htmlText += QString("<p>Разработка по Redmine,%: %1</p>\n").arg( redmineDevelopPrc );
    htmlText += QString("<p>Прочая разработка,ч: %1</p>\n").arg( devData.developOtherHrs() );
    htmlText += QString("<p>Всего ревизий: %1</p>\n").arg( devData.revisionsCount() );
    
    
    std::vector<CRevisionData> largeRevisions = devData.revisionsList( m_LargeRevisionHrsMin );
        
    if( largeRevisions.size() )
    {
      htmlText += "<table border=1>\n";
      htmlText += QString("<caption>Большие ревизии (>%1)</caption>\n").arg( m_LargeRevisionHrsMin );
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
        /*htmlText += QString("<tr><td>%1</td><td> <a href=""http://omp05/git/commit/?r=omp.git&h=aced3c0ba45a104d6661bcef4e551e73c4f6c570"">aced3c0ba45a104d6661bcef4e551e73c4f6c570</a> </td><td>Yes</td> </tr>")
            .arg( revData.HoursSpent() );*/
      }
    }
  }
  
  return true;
}

bool DocumentDataManager::generateWorkData()
{
  m_GenerationDone = false;
  
  GitAnalyzer gitAnalyzer( gitSettings() );
  RedmineAnalyzer redmineAnalyzer( redmineSettings() );
  
  int totalStepsNum = gitAnalyzer.GetAnalyzeStepsCount() + redmineAnalyzer.GetAnalyzeStepsCount();
  
  emit generationStepsNumUpdated( totalStepsNum );
  
  m_GenerationStepNum = 0;
  
  QObject::connect(&gitAnalyzer, &GitAnalyzer::analyzeStepDone, this, &DocumentDataManager::childGenerationStepDone);
  QObject::connect(&redmineAnalyzer, &RedmineAnalyzer::analyzeStepDone, this, &DocumentDataManager::childGenerationStepDone);
  
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
  
  if( !redmineAnalyzer.AnalyzeServer( tempWorkersData, &errStr ) )
  {
    emit generationErrorOccured( QString("ОШИБКА:%1").arg(errStr) );
    return false;
  }
  
  emit generationMessage( "Анализ redmine завершен." );
  
  m_GenerationDone = true;
  m_GenerationResultData = tempWorkersData;
  
  return true;
}

bool DocumentDataManager::CheckSettings(QString &errStr)
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

void DocumentDataManager::SetDateFrom(QDate d)
{
  m_DateFrom = d;
}

void DocumentDataManager::SetDateTo(QDate d)
{
  m_DateTo = d;
}

void DocumentDataManager::SetWorkingDaysQty(unsigned d)
{
  m_WorkingDaysQty = d;
}

void DocumentDataManager::AddWorkingDeveloper( CDeveloperData devData, unsigned holidaysDays )
{
  m_DevelopersWorkDataList.push_back( CDeveloperWorkData(devData, holidaysDays) );
}

bool DocumentDataManager::AreGeneralSettingsVaild(QString *errStr)
{
  if( m_DevelopersManager->Count() == 0 )
  {
    if( errStr )
      *errStr = "Не задан ни один разработчик.";
    
    return false;
  }
  
  if( m_GitRepositoryPath.isEmpty() )
  {
    if( errStr )
      *errStr = "Не задан путь к репозиторю Git.";
    
    return false;
  }
  
  if( m_RedmineURL.isEmpty() )
  {
    if( errStr )
      *errStr = "Не введен redmine URL.";
    
    return false;
  }
  
  if( m_RedmineAuthKey.isEmpty() )
  {
    if( errStr )
      *errStr = "Не введен redmine authentication key.";
    
    return false;
  }
  
  return true;
}

bool DocumentDataManager::SaveGeneralSettings()
{
  QString xmlText;

  if( !MakeGeneralSettingsXML(xmlText) )
    return false;

  QString filePath = getGeneralSettingsFilePath();

  QFile settingsFile( filePath );
  if( !settingsFile.open(QIODevice::WriteOnly) )
    return false;

  QTextStream outStream( &settingsFile );

  outStream << xmlText;

  return true;
}
