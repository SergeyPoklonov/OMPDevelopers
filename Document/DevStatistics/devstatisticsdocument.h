#ifndef DEVSTATISTICSDOCUMENT_H
#define DEVSTATISTICSDOCUMENT_H

#include <QObject>

#include <Document/developerworkdata.h>
#include <Document/generalsettingsholder.h>
#include <Document/gitanalyzer.h>
#include <Document/redmineanalyzer.h>

class DocumentDataManager;

class DevStatisticsDocument : public QObject
{
  Q_OBJECT
public:
  explicit DevStatisticsDocument(QObject *parent = 0);
  
  void clear();
  
  ///////////////////////////////////////////////////////////////////////////
  // data access
  void clearWorkingDevelopers();
  void addWorkingDeveloper( CDeveloperData devData  );

  
  QDate getDateFrom() const;
  QDate getDateTo() const;
  void setDateFrom(QDate d);
  void setDateTo(QDate d);
  
  unsigned getWorkDayLenHrs() const { return WorkHrsInDay; }
  
  unsigned getWorkingDaysQty() const;
  
  bool isRedmineEnabled() const;
  void setRedmineEnabled(bool isEnabled);
  
  bool isLargeRevisionListEnabled() const;
  void setLargeRevisionListEnabled(bool isEnable);
  
  bool isCoreRevisionListEnabled() const;
  void setCoreRevisionListEnabled(bool isEnable);
  QString getCoremodulesNames() const;
  void setCoreModulesNames(const QString &names);
  
  double getMinRevHrs() const;
  void   setMinRevHrs( double hrs );
  
  std::vector< CDeveloperWorkData > getDevelopersStatisticData() const;
  
  QString trackerName(int trackerID) const;
  int issueToTracker(int issueID) const;
  
  OMPCalendarData& getCommonCalendar() { return m_CommonCalendar; }
  const OMPCalendarData& getCommonCalendar() const { return m_CommonCalendar; }
  
  QString getOutputHTMLDefaultFileName() const;

  ///////////////////////////////////////////////////////////////////////////
  // data operations
  bool checkSettings(QString &errStr);
  
  bool generateWorkData();
  
  bool creatHTMLDataFile(QString filePath);
  
  ///////////////////////////////////////////////////////////////////////////
  // signals
signals:
  void generationStepsNumUpdated( int stepsNum );
  void generationStepsDone( int stepsNum );
  void generationMessage( QString msgText );
  void generationErrorOccured( QString errStr );
  
  ///////////////////////////////////////////////////////////////////////////
  // slots
public slots:
  void childGenerationStepDone();
  
private:
  GitAnalyzer::AnalyzeSettings gitSettings();
  RedmineAnalyzer::AnalyzeSettings redmineSettings();
  
  GeneralSettingsHolder& generalSettings();
  DocumentDataManager& parentDocument();
  
private:
  static const unsigned WorkHrsInDay = 8;
  static const unsigned DefaultLargeRevHrsMin = 4;

  QDate m_DateFrom;
  QDate m_DateTo;
  
  bool m_RedmineEnabled;
  
  bool m_LargeRevListEnabled;
  double m_LargeRevisionHrsMin;
  
  bool m_CoreRevListEnabled;
  QString m_CoreModulesNames;
  
  OMPCalendarData m_CommonCalendar;
  
  std::vector< CDeveloperWorkData > m_DevelopersWorkDataList;
  
  bool m_GenerationDone;
  std::vector< CDeveloperWorkData > m_GenerationResultData;
  std::map<long,QString> m_TrackersList;
  std::map<long,long> m_IssuesToTrackers;
  
  int m_GenerationStepNum;
};

#endif // DEVSTATISTICSDOCUMENT_H
