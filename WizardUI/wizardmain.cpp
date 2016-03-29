#include "wizardmain.h"
#include "ui_wizardmain.h"
#include "generalsettingspage.h"

#include <Document/documentdatamanager.h>

WizardMain::WizardMain(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::WizardMain)
{
    ui->setupUi(this);

    m_Document = new DocumentDataManager( this );
}

WizardMain::~WizardMain()
{
    delete ui;
}


bool WizardMain::Initialize()
{
  m_Document->LoadGeneralSettings();

  GeneralSettingsPage *generalSettingsPage = new GeneralSettingsPage( this );

  if( !generalSettingsPage->Initialize( m_Document ) )
    return false;

  addPage( generalSettingsPage );

  return true;
}
