#include "calendarperioddialog.h"

#include <ui_calendarperioddialog.h>

#include "apputils.h"

CalendarPeriodDialog::CalendarPeriodDialog(QDate initialDate, QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::calendarPeriodDialog)
{
  ui->setupUi(this);
  
  initializeForm( initialDate );
}

CalendarPeriodDialog::CalendarPeriodDialog(QWidget *parent)
: QDialog(parent)
, ui(new Ui::calendarPeriodDialog)
{
  ui->setupUi(this);
  
  initializeForm( QDate::currentDate() );
}

CalendarPeriodDialog::~CalendarPeriodDialog()
{
  delete ui;
}

void CalendarPeriodDialog::initializeForm( QDate initialDate )
{
  ui->fromDateCtrl->setDate(initialDate);
  ui->toDateCtrl->setDate(initialDate);
  
  initializeExceptTypesCombo(ui->exceptTypeCombo);
  
  QObject::connect(ui->exceptTypeCombo, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this, &CalendarPeriodDialog::onExceptDayTypeChanged);
}

bool CalendarPeriodDialog::isExcept() const
{
  return ui->exceptDayBox->isChecked();
}

void CalendarPeriodDialog::onExceptDayTypeChanged()
{
  const ExceptDayType newDayType = (ExceptDayType)getComboCurSelData(ui->exceptTypeCombo);
  
  ui->isWorkingDayCheck->setChecked( isWorkDayType( newDayType, ui->isWorkingDayCheck->isChecked() ) );
}

ExceptDayData CalendarPeriodDialog::getExceptDayData() const
{
  if( !isExcept() )
    return ExceptDayData();
  
  ExceptDayData exceptDayData;
  exceptDayData.date = dateFrom();
  exceptDayData.dayType = (ExceptDayType)getComboCurSelData(ui->exceptTypeCombo);
  exceptDayData.isWorking = ui->isWorkingDayCheck->isChecked();
  exceptDayData.name = ui->exceptDayName->text();
  
  return exceptDayData;
}

QDate CalendarPeriodDialog::dateFrom() const
{
  return std::min( ui->fromDateCtrl->date(), ui->toDateCtrl->date() );
}

QDate CalendarPeriodDialog::dateTo() const
{
  return std::max( ui->fromDateCtrl->date(), ui->toDateCtrl->date() );
}
