#include "wizardmain.h"
#include "ui_wizardmain.h"
#include "generalsettingspage.h"
#include "localsettingspage.h"

#include <Document/documentdatamanager.h>

#include <QMessageBox>
#include <QProcess>
#include <QTextCodec>

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

  if( !generalSettingsPage->initialize( m_Document ) )
    return false;

  setPage((int)PAGEID::GENERAL_SETTINGS, generalSettingsPage);

  LocalSettingsPage *localSettingsPage = new LocalSettingsPage( this );

  if( !localSettingsPage->initialize( m_Document ) )
    return false;

  setPage((int)PAGEID::LOCAL_SETTINGS, localSettingsPage);

  return true;
}
