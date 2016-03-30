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
