#ifndef DOCUMENTDATAMANAGER_H
#define DOCUMENTDATAMANAGER_H

#include <QDate>
#include <QObject>
#include "developerworkdata.h"
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

  CDeveloperListDataManager& getDevelopersManager();
  
  void SetGitPath(QString gitPath);
  QString GetGitPath() const;
  
  QString getRedmineURL() const;
  void setRedmineURL(const QString &RedmineURL);
  
  QString getRedmineAuthKey() const;
  void setRedmineAuthKey(const QString &RedmineAuthKey);
  
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

  ///////////////////////////////////////////////////////////////////////////
  // dataGeneration
  bool generateWorkData();
  
  bool creatHTMLDataFile(QString filePath);
  
  QString getOutputHTMLDefaultFilePath() const;
  
signals:
  void gitRepositoryChanged(QString gitPath);
  
  void generationStepsNumUpdated( int stepsNum );
  void generationStepsDone( int stepsNum );
  void generationMessage( QString msgText );
  void generationErrorOccured( QString errStr );

private slots:
  void childGenerationStepDone();

private:
  void Construct();

  bool MakeGeneralSettingsXML(QString &xmlFileText);
  
  GitAnalyzer::AnalyzeSettings gitSettings();
  RedmineAnalyzer::AnalyzeSettings redmineSettings();

private:
  static const int WorkHrsInDay = 8;
  
  CDeveloperListDataManager *m_DevelopersManager;

  QDate m_DateFrom;
  QDate m_DateTo;
  unsigned m_WorkingDaysQty;
  double m_LargeRevisionHrsMin;
  
  QString m_GitRepositoryPath;
  QString m_GitWebURL;
  
  QString m_RedmineURL;
  QString m_RedmineAuthKey;
  
  std::vector< CDeveloperWorkData > m_DevelopersWorkDataList;
  
  bool m_GenerationDone;
  std::vector< CDeveloperWorkData > m_GenerationResultData;
  
  int m_GenerationStepNum;
};

#endif // DOCUMENTDATAMANAGER_H
