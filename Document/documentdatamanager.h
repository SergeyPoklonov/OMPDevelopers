#ifndef DOCUMENTDATAMANAGER_H
#define DOCUMENTDATAMANAGER_H

#include <QDate>
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
  
  void SetGitPath(QString gitPath);
  QString GetGitPath() const;
  
  bool AreGeneralSettingsVaild(QString *errStr = nullptr);

  bool SaveGeneralSettings();
  bool LoadGeneralSettings();

  QString getGeneralSettingsFilePath() const;

  ///////////////////////////////////////////////////////////////////////////
  // localSettings

  void ClearWorkingDevelopers();
  void AddWorkingDeveloper( CDeveloperData devData, unsigned holidaysDays );

  void SetDateFrom(QDate d);
  void SetDateTo(QDate d);
  void SetWorkingDaysQty(unsigned d);

  bool CheckSettings(QString &errStr);

signals:
  void gitRepositoryChanged(QString gitPath);

public slots:

private:
  void Construct();

  bool MakeGeneralSettingsXML(QString &xmlFileText);

private:
  CDeveloperListDataManager *m_DevelopersManager;

  std::vector< CDeveloperWorkData > m_DevelopersWorkDataList;
  QDate m_DateFrom;
  QDate m_DateTo;
  unsigned m_WorkingDaysQty;
  
  QString m_GitRepositoryPath;
};

#endif // DOCUMENTDATAMANAGER_H
