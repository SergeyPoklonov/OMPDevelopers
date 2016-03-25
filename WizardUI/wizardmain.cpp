#include "wizardmain.h"
#include "ui_wizardmain.h"
#include "generalsettingspage.h"

WizardMain::WizardMain(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::WizardMain)
{
    ui->setupUi(this);
}

WizardMain::~WizardMain()
{
    delete ui;
}


bool WizardMain::Initialize()
{
  QWizardPage *generalSettingsPage = new GeneralSettingsPage;

  addPage( generalSettingsPage );

  return true;
}
