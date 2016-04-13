#ifndef DOCUMENTDATAMANAGER_H
#define DOCUMENTDATAMANAGER_H

#include <QDate>
#include <QObject>
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
  
  double GetMinRevHrs() const;
  void   SetMinRevHrs( double hrs );

  bool CheckSettings(QString &errStr);

  ///////////////////////////////////////////////////////////////////////////
  // dataGeneration
  bool generateWorkData();
  
  bool creatHTMLDataFile(QString filePath);
  
  QString getOutputHTMLDefaultFilePath() const;
  
signals:
  void generationStepsNumUpdated( int stepsNum );
  void generationStepsDone( int stepsNum );
  void generationMessage( QString msgText );
  void generationErrorOccured( QString errStr );

private slots:
  void childGenerationStepDone();

private:
  void Construct();
  
  GitAnalyzer::AnalyzeSettings gitSettings();
  RedmineAnalyzer::AnalyzeSettings redmineSettings();
  
  QString makeGitHTMLRevisionURL(const QString &sha);

private:
  static const int WorkHrsInDay = 8;
  static const int DefaultLargeRevHrsMin = 4;

  QDate m_DateFrom;
  QDate m_DateTo;
  unsigned m_WorkingDaysQty;
  double m_LargeRevisionHrsMin;
  
  GeneralSettingsHolder *m_GeneralSettings;
  
  std::vector< CDeveloperWorkData > m_DevelopersWorkDataList;
  
  bool m_GenerationDone;
  std::vector< CDeveloperWorkData > m_GenerationResultData;
  
  int m_GenerationStepNum;
};

#endif // DOCUMENTDATAMANAGER_H
