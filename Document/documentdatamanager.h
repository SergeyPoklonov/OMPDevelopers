#ifndef DOCUMENTDATAMANAGER_H
#define DOCUMENTDATAMANAGER_H

#include <QDate>
#include <QObject>
#include <Document/DevStatistics/devstatisticsdocument.h>
#include "developerworkdata.h"
#include "generalsettingsholder.h"
#include "gitanalyzer.h"
#include "redmineanalyzer.h"

class CDeveloperListDataManager;

class DocumentDataManager : public QObject
{
  Q_OBJECT
public:
  explicit DocumentDataManager(QObject *parent = 0);
  
  void clear();

  ///////////////////////////////////////////////////////////////////////////
  // generalSettings
  
  GeneralSettingsHolder& generalSettings();
  const GeneralSettingsHolder& generalSettings() const;

  bool SaveGeneralSettings();
  bool LoadGeneralSettings();

  QString getGeneralSettingsFilePath() const;

  ///////////////////////////////////////////////////////////////////////////
  // devStatistics
  
  DevStatisticsDocument& devStatistic();
  const DevStatisticsDocument& devStatistic() const;

  QString getOutputHTMLDefaultFilePath() const;
  
signals:

private slots:

private:

private:
  GeneralSettingsHolder *m_GeneralSettings;
  DevStatisticsDocument *m_DevStatisticsDoc;
};

#endif // DOCUMENTDATAMANAGER_H
