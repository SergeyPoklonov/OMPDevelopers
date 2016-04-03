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

void CDeveloperWorkData::setDeveloperData(CDeveloperData devData)
{
  m_DevData = devData;
}

void CDeveloperWorkData::setHolidaysDays(unsigned d)
{
  m_HolidaysDays = d;
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

///////////////////////////////////////////////////////////

void CRevisionData::clear()
{
  m_SHA.clear();
  m_DeveloperName.clear();
  m_HoursSpent = 0.0;
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

void CRevisionData::setHoursSpent(double HoursSpent)
{
    m_HoursSpent = HoursSpent;
}
