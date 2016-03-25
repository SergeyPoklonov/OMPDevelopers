#include "generalsettingspage.h"
#include "ui_generalsettingspage.h"

#include <Document/developerdatamanager.h>

GeneralSettingsPage::GeneralSettingsPage(QWidget *parent) :
  QWizardPage(parent),
  ui(new Ui::GeneralSettingsPage)
{
  ui->setupUi(this);
}

GeneralSettingsPage::~GeneralSettingsPage()
{
  delete ui;
}

bool GeneralSettingsPage::Initialize()
{
  if( !InitializeDevelopersList() )
    return false;

  return true;
}

bool GeneralSettingsPage::InitializeDevelopersList()
{
  CDeveloperListDataManager *devMng = new CDeveloperListDataManager();

  if( !devMng->AddData(CDeveloperData(QString("SPS"), 0.2)) )
    return false;

  if( !devMng->AddData(CDeveloperData(QString("KOV"), 0.5)) )
    return false;

  if( !devMng->AddData(CDeveloperData(QString("AVS"), 0.3)) )
    return false;

  devMng->sort(0, Qt::AscendingOrder);

  ui->developrsList->setModel( devMng );

  return true;
}
