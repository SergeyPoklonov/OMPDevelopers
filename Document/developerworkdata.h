#ifndef CDEVELOPERWORKDATA_H
#define CDEVELOPERWORKDATA_H

#include "developerdata.h"

#include <QString>

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
  void setRedmineLinked(bool RedmineLinked);
  
private:
  QString m_SHA;
  QString m_DeveloperName;
  double  m_HoursSpent;
  bool    m_RedmineLinked;
};

///////////////////////////////////////////////////////////////////////

class CDeveloperWorkData
{
public:
  CDeveloperWorkData();
  CDeveloperWorkData(CDeveloperData devData, unsigned holidaysDays);

public:
  void clear();

  QString getName() const;
  double  getWageRate() const;
  unsigned getHolidaysDays() const;
  
  double redmineTotalHrs() const;
  double redmineDevelopHrs() const;
  double developOtherHrs() const;

  void setDeveloperData(CDeveloperData devData);
  void setHolidaysDays(unsigned d);
  
  std::vector<CRevisionData> revisionsList() const;
  std::vector<CRevisionData> revisionsList( double minTime ) const;
  std::vector<CRevisionData> nonRedmineRevisionsList( bool includeZeroTime ) const;
  size_t revisionsCount() const;
  void addRevision(const CRevisionData &revData);
  
  std::vector<CRedmineTimeData> redmineTimesList() const;
  size_t redmineTimesCount() const;
  void addRedmineTime(const CRedmineTimeData &timeData);

private:
  CDeveloperData m_DevData;
  unsigned m_HolidaysDays;
  std::vector<CRevisionData> m_RevisionsList;
  std::vector<CRedmineTimeData> m_RedmineTimeList;
};

#endif // CDEVELOPERWORKDATA_H
