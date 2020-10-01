#ifndef CDEVELOPERWORKDATA_H
#define CDEVELOPERWORKDATA_H

#include "developerdata.h"

#include <QString>

#include <vector>
#include <set>
#include <map>

class CRedmineTimeData
{
public:
  CRedmineTimeData();
  
  void clear();
  
  long IssueID() const;
  void setIssueID(long IssueID);
  
  double HoursSpent() const;
  void setHoursSpent(double HoursSpent);
  
  bool IsDevelopment() const;
  void setRevision(const QString &revisionSHA);
  
  QString DeveloperName() const;
  void setDeveloperName(const QString &DeveloperName);
  
private:
  long m_IssueID;
  double m_HoursSpent;
  QString m_SHA;
  QString m_DeveloperName;
};

class CRedmineIssueData
{
public:
    CRedmineIssueData();

    void clear();

    long ID() const;
    void setID(long id);

    long State() const;
    void setState(long state);

    QDate CloseDate() const;
    void setCloseDate(QDate d);

    long Tracker() const;
    void setTracker(long tracker);

    long PerformerID() const;
    void setPerformerID(long id);

    bool IsInPlan() const;
    void setIsInPlan(bool b);

    QDate DeadLine() const;
    void setDeadLine(QDate d);

    bool isClosed() const;
    bool isOverdued() const;

    QString Name() const;
    void setName(QString str);

    QString PerformerName() const;
    void setPerformerName(QString str);

    enum customValues
    {
       cvDEADLINE  = 9,
       cvPlan      = 40
    };

    static std::set<long> openStates();

private:
    long m_ID;
    long m_State;
    long m_Tracker;
    long m_PerformerID;
    bool m_IsInPlan;
    QDate m_DeadLine;
    QDate m_CloseDate;
    QString m_Name;
    QString m_AssignedTo;
};

class CRevisionData
{
public:
  CRevisionData();
  
  void clear();
  
  QString SHA() const;
  void setSHA(const QString &SHA);
  
  QString DeveloperName() const;
  void setDeveloperName(const QString &DeveloperName);
  
  double HoursSpent() const;
  void setHoursSpent(double HoursSpent);
  void addHoursSpent(double h);
  
  bool RedmineLinked() const;
  void setRedmineLinked(const std::vector<long> &tasksIds);
  std::vector<long> RedmineTasksIds() const;
  
  bool isChangeCore() const;
  void setChangesCore(bool changes);
  
private:
  QString m_SHA;
  QString m_DeveloperName;
  double  m_HoursSpent;
  std::set<long> m_RedmineTasksIds;
  bool    m_ChangeCore;
};

///////////////////////////////////////////////////////////////////////

class CDeveloperWorkData
{
public:
  CDeveloperWorkData();
  CDeveloperWorkData(CDeveloperData devData);

public:
  void clear();

  QString getName() const;
  double  getWageRate() const;
  
  double redmineTotalHrs() const;
  double redmineDevelopHrs() const;
  double developOtherHrs() const;
  double totalLabourHrs() const { return redmineTotalHrs() + developOtherHrs(); }

  double spentOnPlanHrs( std::map<long,CRedmineIssueData> &issuesData ) const;
  double spentNonPlanHrs( std::map<long,CRedmineIssueData> &issuesData ) const;

  void getIssuesWithHrsSpent( const std::map<long,CRedmineIssueData> &issuesData, std::vector<CRedmineIssueData> &issues, std::map<long, double> &hrsSpentForIssues) const;

  void setDeveloperData(CDeveloperData devData);
  
  std::vector<CRevisionData> revisionsList() const;
  std::vector<CRevisionData> revisionsList( double minTime ) const;
  std::vector<CRevisionData> nonRedmineRevisionsList( bool includeZeroTime ) const;
  std::vector<CRevisionData> coreRevisionsList() const;
  size_t revisionsCount() const;
  void addRevision(const CRevisionData &revData);
  
  OMPCalendarData& calendar() { return m_DevData.calendar(); }
  const OMPCalendarData& calendar() const { return m_DevData.calendar(); }
  
  std::vector<CRedmineTimeData> redmineTimesList() const;
  size_t redmineTimesCount() const;
  void addRedmineTime(const CRedmineTimeData &timeData);

private:
  CDeveloperData m_DevData;
  std::vector<CRevisionData> m_RevisionsList;
  std::vector<CRedmineTimeData> m_RedmineTimeList;
};

#endif // CDEVELOPERWORKDATA_H
