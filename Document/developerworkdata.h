#ifndef CDEVELOPERWORKDATA_H
#define CDEVELOPERWORKDATA_H

#include "developerdata.h"

#include <QString>



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

private:
  CDeveloperData m_DevData;
  unsigned m_HolidaysDays;
};

#endif // CDEVELOPERWORKDATA_H
