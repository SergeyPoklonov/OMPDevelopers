#ifndef CALENDARPERIODDIALOG_H
#define CALENDARPERIODDIALOG_H

#include <QDialog>

#include <Document/Calendar/CalendarData.h>

namespace Ui {
    class calendarPeriodDialog;
} // namespace Ui

class CalendarPeriodDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit CalendarPeriodDialog(QDate initialDate, QWidget *parent = 0);
  explicit CalendarPeriodDialog(QWidget *parent = 0);
  ~CalendarPeriodDialog();
  
  bool isExcept() const;
  ExceptDayData getExceptDayData() const;
  QDate dateFrom() const;
  QDate dateTo() const;
  
public slots:
  void onExceptDayTypeChanged();
  
private:
  void initializeForm( QDate initialDate );
  
private:
  Ui::calendarPeriodDialog *ui;
};

#endif // CALENDARPERIODDIALOG_H
