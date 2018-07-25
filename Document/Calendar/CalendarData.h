#ifndef CALENDARDATA_H
#define CALENDARDATA_H

#include <QDate>
#include <map>
#include <vector>
#include <QDomDocument>

class QComboBox;

enum class ExceptDayType
{
  NONE = 0, // неопределенное значение
  STATEDAY = 1, // государственный праздник
  STATEREWORK = 2, // перенос госпразника
  HOLIDAYS = 3, // отпуск
  SICKLIST = 4 // больничный
};

QString getExceptDayTypeName(ExceptDayType type);

bool isWorkDayType(ExceptDayType type, bool defVal = false);

void initializeExceptTypesCombo(QComboBox *cb);

enum class DaysExcludeMode
{
  LEAVEALL = 0, // не исключать ничего
  STDWORK = 1, // исключать рабочие дни
  STDNONWORK = 2 // исключать выходные и праздники
};


struct ExceptDayData
{
  QDate date;
  QString name;
  bool isWorking;
  ExceptDayType dayType;
  
  ExceptDayData() : isWorking(false), dayType(ExceptDayType::NONE) {}
  ExceptDayData(QDate dayDate, bool isWorkDay, ExceptDayType type, QString dayName) : date(dayDate), isWorking(isWorkDay), name(dayName), dayType(type) {}
  
  // serialize
  void WriteToXML(QDomElement &parentElement);
  void ReadFromXML(QDomElement &parentElement);
};


class OMPCalendarData
{
public:
  OMPCalendarData();
  
  void clear();
  
  // get
  unsigned getWorkDaysQty(QDate from, QDate to) const;
  unsigned getExceptDaysQty(QDate from, QDate to, ExceptDayType dayType, DaysExcludeMode excludeMode = DaysExcludeMode::LEAVEALL) const;
  
  bool isWorkingDay(QDate dayDate) const;
  static bool isWeekend(QDate dayDate);
  bool isStdWork(QDate dayDate) const;
  bool isStdNonwork(QDate dayDate) const;
 
  const ExceptDayData* getExceptionDay(QDate dayDate) const;
  
  std::vector<ExceptDayData> getAllExceptionDays() const;
  
  // set
  void setExceptionDay(QDate date, bool isWorking, ExceptDayType exceptType, QString name = "");
  void setExceptionDay(const ExceptDayData &dayData);
  
  void deleteExceptionDay( QDate date );
  
  // serialize
  void WriteToXML(QDomElement &parentElement);
  void ReadFromXML(QDomElement &parentElement);
  
private:
  void initialize2018();
  
  void setStdExceptionDay(QDate date, bool isWorking, ExceptDayType exceptType, QString name = "");
  
private:
  std::map<QDate,ExceptDayData> m_ExceptionDaysByDates;
  std::map<QDate,ExceptDayData> m_StandartExceptDays;
};

#endif // CALENDARDATA_H
