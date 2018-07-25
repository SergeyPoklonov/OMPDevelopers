#include "developereditor.h"
#include "ui_developereditor.h"
#include "apputils.h"
#include "calendarperioddialog.h"

#include <QMessageBox>

DeveloperEditor::DeveloperEditor(QDate initialDate, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DeveloperEditor),
  m_EditMode( EditMode::insert ),
  m_SelectedDayDataUpdating( false )
{
  ui->setupUi(this);

  initializeForm( initialDate );
}

DeveloperEditor::DeveloperEditor(const CDeveloperData &initialData, QDate initialDate, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DeveloperEditor),
  m_DeveloperData( initialData ),
  m_EditMode( EditMode::update ),
  m_SelectedDayDataUpdating( false )
{
  ui->setupUi(this);

  ui->nameEdit->setText( m_DeveloperData.getName() );
  ui->wageRateEdit->setText( QString::number( m_DeveloperData.getWageRate(), 'f', 2 ) );
  
  initializeForm( initialDate );
}

DeveloperEditor::~DeveloperEditor()
{
  delete ui;
}

void DeveloperEditor::initializeForm( QDate initialDate )
{
  QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(OnOK()));
  
  initializeCalendarWidget( initialDate );
  
  initializeExceptTypesCombo(ui->exceptTypeCombo);
  
  connectExceptDayCtrl();
  
  QObject::connect(ui->setPeriodBtn, &QPushButton::clicked, this, &DeveloperEditor::onSetPeriodClicked);
}

void DeveloperEditor::onSetPeriodClicked()
{
  const QDate selectedDate = ui->calendarWidget->selectedDate();
  
  CalendarPeriodDialog periodDlg( selectedDate, this );
  
  if( periodDlg.exec() == QDialog::Accepted )
  {
    ExceptDayData exceptDayData = periodDlg.getExceptDayData();
    
    for( QDate curDate = periodDlg.dateFrom(); curDate <= periodDlg.dateTo(); curDate = curDate.addDays(1) )
    {
      if( periodDlg.isExcept() )
      {
        exceptDayData.date = curDate;
        
        m_DeveloperData.calendar().setExceptionDay( exceptDayData );
        
        ui->calendarWidget->setDateTextFormat( curDate, getDayFormat( &exceptDayData ) );
      }
      else
      {
        m_DeveloperData.calendar().deleteExceptionDay( curDate );
        
        ui->calendarWidget->setDateTextFormat( curDate, getDayFormat( nullptr ) );
      }
    }
  }
  
  onDateSelected();
}

void DeveloperEditor::initializeCalendarWidget( QDate &initialDate )
{
  QObject::connect(ui->calendarWidget, &QCalendarWidget::selectionChanged, this, &DeveloperEditor::onDateSelected);
  
  std::vector<ExceptDayData> exceptionDays = m_DeveloperData.calendar().getAllExceptionDays();
  
  for( const ExceptDayData &dayData : exceptionDays )
  {
    ui->calendarWidget->setDateTextFormat( dayData.date, getDayFormat( &dayData ) );
  }
  
  ui->calendarWidget->setSelectedDate( initialDate );
  onDateSelected();
}

void DeveloperEditor::onExceptDayPropertiesChanged()
{
  if( m_SelectedDayDataUpdating )
    return;
  
  QDate curDate = ui->calendarWidget->selectedDate();
  
  if( ui->exceptDayBox->isChecked() )
  {
    m_DeveloperData.calendar().setExceptionDay(curDate, ui->isWorkingDayCheck->checkState() == Qt::Checked, 
                                               (ExceptDayType)getComboCurSelData(ui->exceptTypeCombo), 
                                               ui->exceptDayName->text());
  }
  else
  {
    m_DeveloperData.calendar().deleteExceptionDay( curDate );
  }
  
  updateCurDayStyle();
  
  onDateSelected();
}

void DeveloperEditor::updateCurDayStyle()
{
  const QDate &curDate = ui->calendarWidget->selectedDate();
  
  ui->calendarWidget->setDateTextFormat( curDate, getDayFormat( m_DeveloperData.calendar().getExceptionDay( curDate ) ) );
}

void DeveloperEditor::onExceptDayTypeChanged()
{
  if( m_SelectedDayDataUpdating )
    return;
  
  ExceptDayType newDayType = (ExceptDayType)getComboCurSelData(ui->exceptTypeCombo);
  
  ui->isWorkingDayCheck->setChecked( isWorkDayType(newDayType, ui->isWorkingDayCheck->checkState() == Qt::Checked) );
  
  onExceptDayPropertiesChanged();
}

void DeveloperEditor::connectExceptDayCtrl()
{
  QObject::connect(ui->exceptDayBox, &QGroupBox::toggled, this, &DeveloperEditor::onExceptDayPropertiesChanged);
  QObject::connect(ui->isWorkingDayCheck, &QCheckBox::stateChanged, this, &DeveloperEditor::onExceptDayPropertiesChanged);
  QObject::connect(ui->exceptDayName, &QLineEdit::editingFinished, this, &DeveloperEditor::onExceptDayPropertiesChanged);
  QObject::connect(ui->exceptTypeCombo, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this, &DeveloperEditor::onExceptDayTypeChanged);
}

QTextCharFormat DeveloperEditor::getDayFormat(const ExceptDayData *dayData)
{
  if( !dayData )
    return QTextCharFormat();
  
  QTextCharFormat exceptDayFormat;
  
  exceptDayFormat.setUnderlineStyle( QTextCharFormat::SingleUnderline );
  exceptDayFormat.setFontWeight(QFont::Bold);
  
  switch( dayData->dayType ) 
  {
    case ExceptDayType::HOLIDAYS: exceptDayFormat.setForeground( QBrush(Qt::blue) ); break;
    
    case ExceptDayType::SICKLIST: exceptDayFormat.setForeground( QBrush(Qt::darkYellow) ); break;
    
    default:  exceptDayFormat.setForeground( QBrush( dayData->isWorking ? Qt::black : Qt::red) ); break;
  }
  
  return exceptDayFormat;
}

void DeveloperEditor::onDateSelected()
{
  const QDate &date = ui->calendarWidget->selectedDate();
  
  m_SelectedDayDataUpdating = true;
  
  const ExceptDayData *exceptDayData =  m_DeveloperData.calendar().getExceptionDay(date);
  
  if( exceptDayData )
  {
    ui->exceptDayBox->setChecked(true);
    
    ui->dayDateCtrl->setDate(exceptDayData->date);
    
    ui->isWorkingDayCheck->setChecked( m_DeveloperData.calendar().isWorkingDay(date) );
    
    ui->exceptDayName->setText(exceptDayData->name);
    
    selectComboItemByData(ui->exceptTypeCombo, (int)exceptDayData->dayType);
  }
  else
  {
    ui->exceptDayBox->setChecked(false);
    
    ui->dayDateCtrl->setDate(date);
    
    ui->isWorkingDayCheck->setChecked( m_DeveloperData.calendar().isWorkingDay(date) );
    
    ui->exceptDayName->setText("");
    
    selectComboItemByData(ui->exceptTypeCombo, (int)ExceptDayType::NONE);
  }
  
  m_SelectedDayDataUpdating = false;
}

void DeveloperEditor::OnOK()
{
  QString devName = ui->nameEdit->text();

  QString devWageRateStr = ui->wageRateEdit->text();
          devWageRateStr.replace( ",", "." );

  bool conversionOK = false;
  const double devWageRate = devWageRateStr.toDouble( &conversionOK );

  if( !conversionOK )
  {
    QMessageBox::warning(this, "Данные разработчика", "Некорректно задана ставка разработчика. Введите число больше 0." );
    return;
  }

  m_DeveloperData.setName( devName );
  m_DeveloperData.setWageRate( devWageRate );

  QString dataCheckError;

  if( !m_DeveloperData.isValid( &dataCheckError ) )
  {
    QMessageBox msgBox;
                msgBox.setText( dataCheckError );
                msgBox.exec();

    return;
  }

  emit dataAccepted( this );
}
