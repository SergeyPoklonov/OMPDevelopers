#include "developerdatamanager.h"
#include "documentdatamanager.h"

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

QString DocumentDataManager::getGeneralSettingsFilePath() const
{
  QString dirParth = QCoreApplication::instance()->applicationDirPath();
  QString filePath = QString("%1/%2").arg( dirParth ).arg( "OMPDevSettings.xml" );
  return filePath;
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

bool DocumentDataManager::MakeGeneralSettingsXML(QString &xmlFileText)
{
  QDomDocument doc("OMPDevSettings");
  QDomElement root = doc.createElement("SettingsRoot");

  doc.appendChild(root);

  m_DevelopersManager->WriteToXML( root );

  xmlFileText = doc.toString();

  return true;
}
