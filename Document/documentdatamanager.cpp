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
  m_GeneralSettings = new GeneralSettingsHolder( this );
  m_DevStatisticsDoc = new DevStatisticsDocument( this );
}

GeneralSettingsHolder& DocumentDataManager::generalSettings()
{
  return *m_GeneralSettings;
}

DevStatisticsDocument& DocumentDataManager::devStatistic()
{
  return *m_DevStatisticsDoc;
}

void DocumentDataManager::clear()
{
  m_GeneralSettings->clear();
  m_DevStatisticsDoc->clear();
}

QString DocumentDataManager::getOutputHTMLDefaultFilePath() const
{
  const QString fileName = m_DevStatisticsDoc->getOutputHTMLDefaultFileName();
  
  const QString dirParth = QCoreApplication::instance()->applicationDirPath();
  
  const QString filePath = QString("%1/%2").arg( dirParth ).arg( fileName );
  
  return filePath;
}

QString DocumentDataManager::getGeneralSettingsFilePath() const
{
  QString dirParth = QCoreApplication::instance()->applicationDirPath();
  QString filePath = QString("%1/%2").arg( dirParth ).arg( "OMPDevSettings.xml" );
  return filePath;
}

bool DocumentDataManager::LoadGeneralSettings()
{
  return generalSettings().load( getGeneralSettingsFilePath() );
}

bool DocumentDataManager::SaveGeneralSettings()
{
  return generalSettings().save( getGeneralSettingsFilePath() );
}
