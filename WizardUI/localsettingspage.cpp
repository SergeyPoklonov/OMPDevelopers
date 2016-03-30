#include "localsettingspage.h"
#include "ui_localsettingspage.h"

#include <Document/developerdatamanager.h>
#include <Document/documentdatamanager.h>

#include <QMessageBox>

LocalSettingsPage::LocalSettingsPage(QWidget *parent) :
  QWizardPage(parent),
  ui(new Ui::LocalSettingsPage)
{
  ui->setupUi(this);
}

LocalSettingsPage::~LocalSettingsPage()
{
  delete ui;
}

void LocalSettingsPage::onDeveloperDataChanged( QTableWidgetItem *item )
{
  if( item->column() == DEVLIST_COLNUM_HDAYS )
  {
    QVariant val = item->data( Qt::DisplayRole );

    const int newDaysVal = val.toInt();
    if( newDaysVal <= 0 )
    {
      item->setData( Qt::DisplayRole, QVariant() );
    }
  }
}

bool LocalSettingsPage::validatePage()
{
  saveDevelopersToDocument();

  saveWorkPeriodToDocument();

  QString str;
  if( !getDocument().CheckSettings(str) )
  {
    QMessageBox::warning(this, "Настройки", str);
    return false;
  }

  return QWizardPage::validatePage();
}

void LocalSettingsPage::initializeWorkPeriodCtrls()
{
  const QDate curDate = QDate::currentDate();

  QDate endDate( curDate.year(), curDate.month(), 1 );
  endDate = endDate.addDays(-1);

  ui->dateTo->setDate( endDate );

  QDate begDate = endDate;
  begDate = begDate.addDays( (endDate.daysInMonth()-1) * (-1) );

  ui->dateFrom->setDate( begDate );

  ui->workingDays->setMinimum(1);
  ui->workingDays->setValue( begDate.daysTo(endDate) );
}

void LocalSettingsPage::saveWorkPeriodToDocument()
{
  getDocument().SetDateFrom( ui->dateFrom->date() );
  getDocument().SetDateTo( ui->dateTo->date() );
  getDocument().SetWorkingDaysQty( ui->workingDays->value() );
}

void LocalSettingsPage::saveDevelopersToDocument()
{
  getDocument().ClearWorkingDevelopers();

  const int rowsQty = ui->developersList->rowCount();
  for(int row = 0; row < rowsQty; row++)
  {
    QTableWidgetItem *nameItem = ui->developersList->item(row, DEVLIST_COLNUM_NAME);

    if( nameItem->checkState() == Qt::Unchecked )
      continue;

    QString devName = nameItem->data(Qt::DisplayRole).toString();

    CDeveloperData devData = getDocument().getDevelopersManager().GetDataByName( devName );

    Q_ASSERT( devData.isValid() );

    if( !devData.isValid() )
      continue;

    QTableWidgetItem *hdaysItem = ui->developersList->item(row, DEVLIST_COLNUM_HDAYS);

    unsigned holidaysDays = hdaysItem->data(Qt::DisplayRole).toUInt();

    getDocument().AddWorkingDeveloper( devData, holidaysDays );
  }
}

void LocalSettingsPage::intializeDevelopersList()
{
  const CDeveloperListDataManager &developersMng = getDocument().getDevelopersManager();

  ui->developersList->setColumnCount( DEVLIST_COLSQTY );
  ui->developersList->setHorizontalHeaderLabels( {QString("Имя"), QString("Отпуск,дн")} );

  ui->developersList->setRowCount( developersMng.Count() );

  for(size_t i = 0; i < developersMng.Count(); i++)
  {
    CDeveloperData devData = developersMng.GetDataByInd( i );

    QTableWidgetItem *nameItem = new QTableWidgetItem();
                      nameItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
                      nameItem->setData(Qt::DisplayRole, devData.getName());
                      nameItem->setData(Qt::CheckStateRole, (int)Qt::Checked);
    ui->developersList->setItem(i, DEVLIST_COLNUM_NAME, nameItem);

    QTableWidgetItem *holidaysItem = new QTableWidgetItem( );
                      holidaysItem->setFlags( Qt::ItemIsEditable | Qt::ItemIsEnabled );
                      holidaysItem->setTextAlignment( Qt::AlignHCenter );
    ui->developersList->setItem(i, DEVLIST_COLNUM_HDAYS, holidaysItem);
  }

  QObject::connect(ui->developersList, &QTableWidget::itemChanged, this, &LocalSettingsPage::onDeveloperDataChanged);
}

void LocalSettingsPage::initializePage()
{
  QWizardPage::initializePage();

  intializeDevelopersList();
}

bool LocalSettingsPage::initialize( DocumentDataManager *doc )
{
  if( !CWizardPageDocumentSupport::initialize( doc ) )
    return false;

  initializeWorkPeriodCtrls();

  return true;
}
