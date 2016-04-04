#include "generationpage.h"
#include "ui_generationpage.h"

GenerationPage::GenerationPage(QWidget *parent) :
  QWizardPage(parent),
  ui(new Ui::GenerationPage)
{
  ui->setupUi(this);
}

GenerationPage::~GenerationPage()
{
  delete ui;
}

bool GenerationPage::initialize( DocumentDataManager *doc )
{
  if( !CWizardPageDocumentSupport::initialize( doc ) )
    return false;
  
  return true;
}
