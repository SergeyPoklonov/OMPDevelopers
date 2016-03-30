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

void DocumentDataManager::ClearWorkingDevelopers()
{
  m_DevelopersWorkDataList.clear();
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
