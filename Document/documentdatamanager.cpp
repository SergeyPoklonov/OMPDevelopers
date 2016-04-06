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

bool DocumentDataManager::generateWorkData()
{
  emit generationStepsNumUpdated( 3 );
  
  std::vector< CDeveloperWorkData > tempWorkersData = m_DevelopersWorkDataList;
  
  int stepNum = 0;
  
  // git
  emit generationMessage( "Анализ репозитория git..." );
  
  GitAnalyzer gitAnalyzer( gitSettings() );
  QString errStr;
    
  if( !gitAnalyzer.AnalyzeRepository( tempWorkersData, &errStr ) )
  {
    emit generationErrorOccured( QString("ОШИБКА:%1").arg(errStr) );
    return false;
  }
  
  emit generationMessage( "Анализ репозитория git завершен." );
  emit generationStepsDone( ++stepNum );
  
  // redmine
  emit generationMessage( "Анализ redmine..." );
  
  RedmineAnalyzer redmineAnalyzer( redmineSettings() );
  
  if( !redmineAnalyzer.AnalyzeServer( tempWorkersData, &errStr ) )
  {
    emit generationErrorOccured( QString("ОШИБКА:%1").arg(errStr) );
    return false;
  }
  
  emit generationMessage( "Анализ redmine завершен." );
  emit generationStepsDone( ++stepNum );
  
  // workers
  
  m_GenerationResult = tempWorkersData;
  
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
