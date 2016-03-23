#include "wizardmain.h"
#include "ui_wizardmain.h"

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
