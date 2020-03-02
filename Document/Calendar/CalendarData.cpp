#include "CalendarData.h"

#include <QComboBox>


OMPCalendarData::OMPCalendarData()
{
  initialize2018();
  initialize2020();
}

void OMPCalendarData::clear()
{
  m_ExceptionDaysByDates.clear();
}

std::vector<ExceptDayData> OMPCalendarData::getAllExceptionDays() const
{
  std::vector<ExceptDayData> retData;
  
  for(auto &dayItr : m_ExceptionDaysByDates)
  {
    retData.push_back( dayItr.second );
  }
  
  return retData;
}

const ExceptDayData* OMPCalendarData::getExceptionDay(QDate dayDate) const
{
  auto findItr = m_ExceptionDaysByDates.find( dayDate );
  
  if( findItr != m_ExceptionDaysByDates.end() )
    return &((*findItr).second);
  
  return nullptr;
}

unsigned OMPCalendarData::getExceptDaysQty(QDate from, QDate to, ExceptDayType dayType, DaysExcludeMode excludeMode) const
{
  Q_ASSERT(from.isValid() && to.isValid());
  
  unsigned daysQty = 0;
  
  for(QDate curDate = std::min(from,to); curDate < std::max(from,to); curDate = curDate.addDays(1))
  {
    const ExceptDayData *exceptDayData = getExceptionDay(curDate);
    
    if( exceptDayData && exceptDayData->dayType == dayType )
    {
      if( excludeMode == DaysExcludeMode::LEAVEALL ||
          (excludeMode == DaysExcludeMode::STDWORK && !isStdWork(curDate)) ||
          (excludeMode == DaysExcludeMode::STDNONWORK && !isStdNonwork(curDate)) )
      {
        daysQty++;
      }
    }
  }
  
  return daysQty;
}

unsigned OMPCalendarData::getWorkDaysQty(QDate from, QDate to) const
{
  Q_ASSERT(from.isValid() && to.isValid());
  
  unsigned workDaysQty = 0;
  
  for(QDate curDate = std::min(from,to); curDate < std::max(from,to); curDate = curDate.addDays(1))
  {
    if( isWorkingDay(curDate) )
      workDaysQty++;
  }
  
  return workDaysQty;
}

void OMPCalendarData::setExceptionDay(QDate date, bool isWorking, ExceptDayType exceptType, QString name)
{
  Q_ASSERT( date.isValid() );
  
  m_ExceptionDaysByDates[date] = ExceptDayData(date, isWorking, exceptType, name);
}

void OMPCalendarData::setStdExceptionDay(QDate date, bool isWorking, ExceptDayType exceptType, QString name)
{
  Q_ASSERT( date.isValid() );
  
  m_StandartExceptDays[date] = ExceptDayData(date, isWorking, exceptType, name);
}

void OMPCalendarData::setExceptionDay(const ExceptDayData &dayData)
{
  m_ExceptionDaysByDates[dayData.date] = dayData;
}

void OMPCalendarData::deleteExceptionDay( QDate date )
{
  m_ExceptionDaysByDates.erase( date );
}

bool OMPCalendarData::isWeekend(QDate dayDate)
{
  return dayDate.dayOfWeek() == 6 || dayDate.dayOfWeek() == 7;
}

bool OMPCalendarData::isStdWork(QDate dayDate) const
{
  bool isWorkDay = !isWeekend(dayDate);
  
  auto findItr = m_StandartExceptDays.find( dayDate );
  
  if( findItr != m_StandartExceptDays.end() )
    isWorkDay = (*findItr).second.isWorking;
  
  return isWorkDay;
}

bool OMPCalendarData::isStdNonwork(QDate dayDate) const
{
  return !isStdWork(dayDate);
}

bool OMPCalendarData::isWorkingDay(QDate dayDate) const
{
  bool isWorkDay = isStdWork(dayDate);
  
  auto findItr = m_ExceptionDaysByDates.find( dayDate );
  
  if( findItr != m_ExceptionDaysByDates.end() )
    isWorkDay = (*findItr).second.isWorking;
  
  return isWorkDay;
}

void OMPCalendarData::initialize2018()
{
  setStdExceptionDay(QDate(2018,1,2), false, ExceptDayType::STATEDAY, "Новый Год");
  setStdExceptionDay(QDate(2018,1,1), false, ExceptDayType::STATEDAY, "Новый Год");
  setStdExceptionDay(QDate(2018,1,20), true, ExceptDayType::STATEREWORK, "Отработка Новый Год");
  setStdExceptionDay(QDate(2018,3,3), true, ExceptDayType::STATEREWORK, "Отработка 8 Марта");
  setStdExceptionDay(QDate(2018,3,8), false, ExceptDayType::STATEDAY, "8 Марта");
  setStdExceptionDay(QDate(2018,3,9), false, ExceptDayType::STATEDAY, "8 Марта");
  setStdExceptionDay(QDate(2018,4,14), true, ExceptDayType::STATEREWORK, "Отработка Радоница");
  setStdExceptionDay(QDate(2018,4,16), false, ExceptDayType::STATEDAY, "Радоница");
  setStdExceptionDay(QDate(2018,4,17), false, ExceptDayType::STATEDAY, "Радоница");
  setStdExceptionDay(QDate(2018,4,28), true, ExceptDayType::STATEREWORK, "Отработка 1 Мая");
  setStdExceptionDay(QDate(2018,4,30), false, ExceptDayType::STATEDAY, "1 Мая");
  setStdExceptionDay(QDate(2018,5,1), false, ExceptDayType::STATEDAY, "1 Мая");
  setStdExceptionDay(QDate(2018,5,9), false, ExceptDayType::STATEDAY, "9 Мая");
  setStdExceptionDay(QDate(2018,7,2), false, ExceptDayType::STATEDAY, "День Независимости");
  setStdExceptionDay(QDate(2018,7,3), false, ExceptDayType::STATEDAY, "День Независимости");
  setStdExceptionDay(QDate(2018,7,7), true, ExceptDayType::STATEREWORK, "Отработка День Независимости");
  setStdExceptionDay(QDate(2018,11,7), false, ExceptDayType::STATEDAY, "День ВОСР");
  setStdExceptionDay(QDate(2018,12,22), true, ExceptDayType::STATEREWORK, "Отработка Рождество");
  setStdExceptionDay(QDate(2018,12,24), false, ExceptDayType::STATEDAY, "Рождество");
  setStdExceptionDay(QDate(2018,12,25), false, ExceptDayType::STATEDAY, "Рождество");
  setStdExceptionDay(QDate(2018,12,29), true, ExceptDayType::STATEREWORK, "Отработка Новый Год");
  setStdExceptionDay(QDate(2018,12,31), false, ExceptDayType::STATEDAY, "Новый Год");
}

void OMPCalendarData::initialize2020()
{
  setStdExceptionDay(QDate(2018,1,1), false, ExceptDayType::STATEDAY, "Новый Год");
  setStdExceptionDay(QDate(2018,1,2), false, ExceptDayType::STATEDAY, "Новый Год");
  setStdExceptionDay(QDate(2018,1,4), true, ExceptDayType::STATEREWORK, "Отработка Рождество");
  setStdExceptionDay(QDate(2018,1,6), false, ExceptDayType::STATEDAY, "Рождество");
  setStdExceptionDay(QDate(2018,1,7), false, ExceptDayType::STATEDAY, "Рождество");
  setStdExceptionDay(QDate(2018,4,4), true, ExceptDayType::STATEREWORK, "Отработка Радоница");
  setStdExceptionDay(QDate(2018,4,27), false, ExceptDayType::STATEDAY, "Рождество");
  setStdExceptionDay(QDate(2018,4,28), false, ExceptDayType::STATEDAY, "Рождество");
  setStdExceptionDay(QDate(2018,5,1), false, ExceptDayType::STATEDAY, "1 Мая");
  setStdExceptionDay(QDate(2018,7,3), false, ExceptDayType::STATEDAY, "День Независимости");
  setStdExceptionDay(QDate(2018,12,25), false, ExceptDayType::STATEDAY, "Рождество");
}

void OMPCalendarData::WriteToXML(QDomElement &parentElement)
{
  if( m_ExceptionDaysByDates.empty() )
    return;
  
  QDomDocument doc = parentElement.ownerDocument();

  QDomElement baseElement = doc.createElement("calendar");
  parentElement.appendChild( baseElement );

  baseElement.setAttribute( "ExceptionDaysQty", m_ExceptionDaysByDates.size() );
  
  int dayInd = 0;
  for( auto dayPair : m_ExceptionDaysByDates )
  {
    QDomElement dayElement = doc.createElement( QString("ExceptDay%1").arg(dayInd+1) );
    baseElement.appendChild( dayElement );

    const ExceptDayData &dayData = dayPair.second;
    
    dayElement.setAttribute("date", dayData.date.toJulianDay());
    dayElement.setAttribute("isWorkOn", dayData.isWorking ? 1 : 0 );
    dayElement.setAttribute("dayType", (int)dayData.dayType);
    dayElement.setAttribute("note", dayData.name);
    
    dayInd++;
  }
}

void OMPCalendarData::ReadFromXML(QDomElement &parentElement)
{
  clear();
  
  QDomElement baseElement = parentElement.firstChildElement( "calendar" );
  
  if( !baseElement.isNull() )
  {
    size_t daysQty = baseElement.attribute( "ExceptionDaysQty" ).toUInt();

    for(size_t i = 0; i < daysQty; i++)
    {
      QDomElement dayElement = baseElement.firstChildElement( QString("ExceptDay%1").arg(i+1) );

      if( !dayElement.isNull() )
      {
        ExceptDayData dayData;
        
        const qint64 jDate = dayElement.attribute("date").toLongLong();
        
        dayData.date = QDate::fromJulianDay( jDate );
        
        if( !dayData.date.isValid() )
          continue;

        dayData.isWorking = dayElement.attribute("isWorkOn").toInt() != 0;
        dayData.dayType = (ExceptDayType)( dayElement.attribute("dayType").toInt() );
        dayData.name = dayElement.attribute("note");
        
        setExceptionDay( dayData );
      }
    }
  }
}

QString getExceptDayTypeName(ExceptDayType type)
{
  QString name;
  
  switch( type )
  {
    case ExceptDayType::NONE: break;
    case ExceptDayType::STATEDAY: name = "Государственный праздник"; break;
    case ExceptDayType::STATEREWORK: name = "Отработка за госпраздник"; break;
    case ExceptDayType::HOLIDAYS: name = "Отпуск"; break;
    case ExceptDayType::SICKLIST: name = "Больничный"; break;
  }
  return name;
}

bool isWorkDayType(ExceptDayType type, bool defVal)
{
  bool isWorking = false;
  
  switch( type )
  {
    case ExceptDayType::NONE: isWorking = defVal;
    case ExceptDayType::STATEDAY: isWorking = false; break;
    case ExceptDayType::STATEREWORK: isWorking = true; break;
    case ExceptDayType::HOLIDAYS: isWorking = false; break;
    case ExceptDayType::SICKLIST: isWorking = false; break;
    default: isWorking = defVal; break;
  }
  
  return isWorking;
}


void initializeExceptTypesCombo(QComboBox *cb)
{
  cb->addItem(getExceptDayTypeName(ExceptDayType::NONE), (int)ExceptDayType::NONE);
  cb->addItem(getExceptDayTypeName(ExceptDayType::STATEDAY), (int)ExceptDayType::STATEDAY);
  cb->addItem(getExceptDayTypeName(ExceptDayType::STATEREWORK), (int)ExceptDayType::STATEREWORK);
  cb->addItem(getExceptDayTypeName(ExceptDayType::HOLIDAYS), (int)ExceptDayType::HOLIDAYS);
  cb->addItem(getExceptDayTypeName(ExceptDayType::SICKLIST), (int)ExceptDayType::SICKLIST);
}
