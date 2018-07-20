#ifndef DEVSTATISTICSDOCUMENT_H
#define DEVSTATISTICSDOCUMENT_H

#include <QObject>

#include <Document/developerworkdata.h>
#include <Document/generalsettingsholder.h>
#include <Document/gitanalyzer.h>
#include <Document/redmineanalyzer.h>

class DevStatisticsDocument : public QObject
{
  Q_OBJECT
public:
  explicit DevStatisticsDocument(QObject *parent = 0);
  
  void clear();
  
  ///////////////////////////////////////////////////////////////////////////
  // settings
  void clearWorkingDevelopers();
  void addWorkingDeveloper( CDeveloperData devData, unsigned holidaysDays );

  void setDateFrom(QDate d);
  void setDateTo(QDate d);
  void setWorkingDaysQty(unsigned d);
  
  double getMinRevHrs() const;
  void   setMinRevHrs( double hrs );

  bool checkSettings(QString &errStr);
  
  bool generateWorkData();
  
  bool creatHTMLDataFile(QString filePath);
  
  QString getOutputHTMLDefaultFileName() const;
  
signals:
  void generationStepsNumUpdated( int stepsNum );
  void generationStepsDone( int stepsNum );
  void generationMessage( QString msgText );
  void generationErrorOccured( QString errStr );
  
public slots:
  void childGenerationStepDone();
  
private:
  GitAnalyzer::AnalyzeSettings gitSettings();
  RedmineAnalyzer::AnalyzeSettings redmineSettings();
  
  QString makeGitHTMLRevisionURL(const QString &sha);
  
  GeneralSettingsHolder& generalSettings();
  
private:
  static const int WorkHrsInDay = 8;
  static const int DefaultLargeRevHrsMin = 4;

  QDate m_DateFrom;
  QDate m_DateTo;
  unsigned m_WorkingDaysQty;
  double m_LargeRevisionHrsMin;
  
  std::vector< CDeveloperWorkData > m_DevelopersWorkDataList;
  
  bool m_GenerationDone;
  std::vector< CDeveloperWorkData > m_GenerationResultData;
  std::map<long,QString> m_TrackersList;
  std::map<long,long> m_IssuesToTrackers;
  
  int m_GenerationStepNum;
};

#endif // DEVSTATISTICSDOCUMENT_H
