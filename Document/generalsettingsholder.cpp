#include "generalsettingsholder.h"

#include <QFile>
#include <QTextStream>

GeneralSettingsHolder::GeneralSettingsHolder(QObject *parent) 
  : QObject(parent)
{
  m_DevelopersManager = new CDeveloperListDataManager( this );
}

void GeneralSettingsHolder::clear()
{
  m_DevelopersManager->clear();
  m_GitWebURL.clear();
  m_GitRepositoryPath.clear();
  m_RedmineURL.clear();
  m_RedmineAuthKey.clear();
}

QString GeneralSettingsHolder::getRedmineAuthKey() const
{
  return m_RedmineAuthKey;
}

void GeneralSettingsHolder::setRedmineAuthKey(const QString &RedmineAuthKey)
{
  m_RedmineAuthKey = RedmineAuthKey;
}

QString GeneralSettingsHolder::getRedmineURL() const
{
  return m_RedmineURL;
}

void GeneralSettingsHolder::setRedmineURL(const QString &RedmineURL)
{
  m_RedmineURL = RedmineURL;
}

void GeneralSettingsHolder::setGitWeb(QString url)
{
  m_GitWebURL = url;
}

QString GeneralSettingsHolder::getGitWeb() const
{
  return m_GitWebURL; 
}

void GeneralSettingsHolder::setGitPath(QString gitPath)
{
  m_GitRepositoryPath = gitPath;
}

QString GeneralSettingsHolder::getGitPath() const
{
  return m_GitRepositoryPath;
}

CDeveloperListDataManager& GeneralSettingsHolder::getDevelopersManager()
{
  return *m_DevelopersManager;
}

bool GeneralSettingsHolder::MakeXMLText(QString &xmlFileText)
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
  gitElement.setAttribute( "GitWeb", m_GitWebURL );
  
  // redmine
  QDomElement redmineElement = doc.createElement("Redmine");
  
  root.appendChild( redmineElement );
  
  redmineElement.setAttribute( "URL", m_RedmineURL );
  redmineElement.setAttribute( "Key", m_RedmineAuthKey );

  xmlFileText = doc.toString();

  return true;
}

bool GeneralSettingsHolder::ParseXMLFile(QFile *xmlFile)
{
  QDomDocument doc("OMPDevSettings");

  if( !doc.setContent(xmlFile) )
    return false;

  QDomElement root = doc.documentElement();

  if( root.isNull() )
    return false;

  m_DevelopersManager->LoadFromXML( root );
  
  QDomElement gitElement = root.firstChildElement( "Git" );
  
  m_GitRepositoryPath = gitElement.attribute( "RepoPath" );
  m_GitWebURL = gitElement.attribute("GitWeb");
  
  QDomElement redmineElement = root.firstChildElement( "Redmine" );
  
  m_RedmineURL = redmineElement.attribute("URL");
  m_RedmineAuthKey = redmineElement.attribute("Key");

  return true;
}

bool GeneralSettingsHolder::save(const QString &filePath)
{
  QString xmlText;

  if( !MakeXMLText(xmlText) )
    return false;

  QFile settingsFile( filePath );
  if( !settingsFile.open(QIODevice::WriteOnly) )
    return false;

  QTextStream outStream( &settingsFile );

  outStream << xmlText;

  return true;
}

bool GeneralSettingsHolder::load(const QString &filePath)
{
  clear();

  QFile settingsFile( filePath );
  if( !settingsFile.open(QIODevice::ReadOnly) )
    return false;
  
  if( ParseXMLFile( &settingsFile ) )
  {
    settingsFile.close();
    return false;
  }

  return true;
}

bool GeneralSettingsHolder::isVaild(QString *errStr)
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
