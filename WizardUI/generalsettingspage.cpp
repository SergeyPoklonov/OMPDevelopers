#include "generalsettingspage.h"
#include "ui_generalsettingspage.h"

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

void GeneralSettingsPage::onDevelopersListSelChanged(const QModelIndexList &indexes)
{
  Q_UNUSED( indexes );
}
