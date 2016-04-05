#ifndef CDEVELOPERWORKDATA_H
#define CDEVELOPERWORKDATA_H

#include "developerdata.h"

#include <QString>


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

  void setDeveloperData(CDeveloperData devData);
  void setHolidaysDays(unsigned d);
  
  std::vector<CRevisionData> revisionsList() const;
  size_t revisionsCount() const;
  
  void addRevision(const CRevisionData &revData);

private:
  CDeveloperData m_DevData;
  unsigned m_HolidaysDays;
  std::vector<CRevisionData> m_RevisionsList;
};

#endif // CDEVELOPERWORKDATA_H
