#ifndef DOCUMENTDATAMANAGER_H
#define DOCUMENTDATAMANAGER_H

#include <QObject>
#include "developerworkdata.h"

class CDeveloperListDataManager;

class DocumentDataManager : public QObject
{
  Q_OBJECT
public:
  explicit DocumentDataManager(QObject *parent = 0);

  ///////////////////////////////////////////////////////////////////////////
  // generalSettings

  CDeveloperListDataManager& getDevelopersManager();

  bool SaveGeneralSettings();
  bool LoadGeneralSettings();

  QString getGeneralSettingsFilePath() const;

  ///////////////////////////////////////////////////////////////////////////
  // localSettings

  void ClearWorkingDevelopers();
  void AddWorkingDeveloper( CDeveloperData devData, unsigned holidaysDays );

  bool CheckSettings(QString &errStr);

signals:

public slots:

private:
  void Construct();

  bool MakeGeneralSettingsXML(QString &xmlFileText);

private:
  CDeveloperListDataManager *m_DevelopersManager;
  std::vector< CDeveloperWorkData > m_DevelopersWorkDataList;
};

#endif // DOCUMENTDATAMANAGER_H
