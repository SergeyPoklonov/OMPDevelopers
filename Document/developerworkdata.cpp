#include "developerworkdata.h"

CDeveloperWorkData::CDeveloperWorkData()
{
  clear();
}

CDeveloperWorkData::CDeveloperWorkData(CDeveloperData devData, unsigned holidaysDays)
{
  clear();
  setDeveloperData( devData );
  setHolidaysDays(holidaysDays);
}

void CDeveloperWorkData::clear()
{
  m_DevData.clear();
  m_HolidaysDays = 0;
  m_RevisionsList.clear();
  m_RedmineTimeList.clear();
}

QString CDeveloperWorkData::getName() const
{
  return m_DevData.getName();
}

double CDeveloperWorkData::getWageRate() const
{
  return m_DevData.getWageRate();
}

unsigned CDeveloperWorkData::getHolidaysDays() const
{
  return m_HolidaysDays;
}

double CDeveloperWorkData::developOtherHrs() const
{
  return std::accumulate(m_RevisionsList.begin(), m_RevisionsList.end(), 0.0, [](double val, const CRevisionData &rev)->double
  {
    if( !rev.RedmineLinked() )
      val += rev.HoursSpent();
    
    return val;
  });
}

double CDeveloperWorkData::redmineDevelopHrs() const
{
  return std::accumulate(m_RedmineTimeList.begin(), m_RedmineTimeList.end(), 0.0, [](double val, const CRedmineTimeData &td)->double
  {
    if( td.IsDevelopment() )
      val += td.HoursSpent();
    
    return val;
  });
}

double CDeveloperWorkData::redmineTotalHrs() const
{
  return std::accumulate(m_RedmineTimeList.begin(), m_RedmineTimeList.end(), 0.0, [](double val, const CRedmineTimeData &td)->double
  {
    return val + td.HoursSpent();
  });
}

void CDeveloperWorkData::setDeveloperData(CDeveloperData devData)
{
  m_DevData = devData;
}

void CDeveloperWorkData::setHolidaysDays(unsigned d)
{
  m_HolidaysDays = d;
}

std::vector<CRevisionData> CDeveloperWorkData::revisionsList( double minTime ) const
{
  std::vector<CRevisionData> retList;
  for( const CRevisionData &rev : m_RevisionsList )
  {
    if( rev.HoursSpent() >= minTime )
      retList.push_back( rev );
  }
  
  return retList;
}

std::vector<CRevisionData> CDeveloperWorkData::revisionsList() const
{
  return m_RevisionsList;
}

size_t CDeveloperWorkData::revisionsCount() const
{
  return m_RevisionsList.size();
}

void CDeveloperWorkData::addRevision(const CRevisionData &revData)
{
  m_RevisionsList.push_back( revData );
}

std::vector<CRedmineTimeData> CDeveloperWorkData::redmineTimesList() const
{
  return m_RedmineTimeList;
}

size_t CDeveloperWorkData::redmineTimesCount() const
{
  return m_RedmineTimeList.size();
}

void CDeveloperWorkData::addRedmineTime(const CRedmineTimeData &timeData)
{
  m_RedmineTimeList.push_back( timeData ); 
}

///////////////////////////////////////////////////////////

void CRevisionData::clear()
{
  m_SHA.clear();
  m_DeveloperName.clear();
  m_HoursSpent = 0.0;
  m_RedmineLinked = false;
}

CRevisionData::CRevisionData()
{
  clear();
}

QString CRevisionData::SHA() const
{
    return m_SHA;
}

void CRevisionData::setSHA(const QString &SHA)
{
    m_SHA = SHA;
}

QString CRevisionData::DeveloperName() const
{
    return m_DeveloperName;
}

void CRevisionData::setDeveloperName(const QString &DeveloperName)
{
    m_DeveloperName = DeveloperName;
}

double CRevisionData::HoursSpent() const
{
    return m_HoursSpent;
}

void CRevisionData::addHoursSpent(double h)
{
  m_HoursSpent += h;
}

void CRevisionData::setHoursSpent(double HoursSpent)
{
    m_HoursSpent = HoursSpent;
}

bool CRevisionData::RedmineLinked() const
{
    return m_RedmineLinked;
}

void CRevisionData::setRedmineLinked(bool RedmineLinked)
{
    m_RedmineLinked = RedmineLinked;
}

///////////////////////////////////////////////////////////////////////////

CRedmineTimeData::CRedmineTimeData()
{
  clear();
}

void CRedmineTimeData::clear()
{
  m_IssueID = 0;
  m_HoursSpent = 0.0;
  m_SHA.clear();
  m_DeveloperName.clear();
}

long CRedmineTimeData::IssueID() const
{
    return m_IssueID;
}

void CRedmineTimeData::setIssueID(long IssueID)
{
    m_IssueID = IssueID;
}

double CRedmineTimeData::HoursSpent() const
{
    return m_HoursSpent;
}

void CRedmineTimeData::setHoursSpent(double HoursSpent)
{
    m_HoursSpent = HoursSpent;
}

bool CRedmineTimeData::IsDevelopment() const
{
    return !m_SHA.isEmpty();
}

void CRedmineTimeData::setRevision(const QString &revisionSHA)
{
  m_SHA = revisionSHA;
}

QString CRedmineTimeData::DeveloperName() const
{
    return m_DeveloperName;
}

void CRedmineTimeData::setDeveloperName(const QString &DeveloperName)
{
    m_DeveloperName = DeveloperName;
}
