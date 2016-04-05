#include "generationpage.h"
#include "Document/documentdatamanager.h"
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

void GenerationPage::initializePage()
{
  QWizardPage::initializePage();
  
  getDocument().generateWorkData();
}

bool GenerationPage::initialize( DocumentDataManager *doc )
{
  if( !CWizardPageDocumentSupport::initialize( doc ) )
    return false;
  
  QObject::connect(&getDocument(), &DocumentDataManager::generationStepsNumUpdated, ui->progressBar, &QProgressBar::setMaximum);
  QObject::connect(&getDocument(), &DocumentDataManager::generationStepsDone, ui->progressBar, &QProgressBar::setValue);
  QObject::connect(&getDocument(), &DocumentDataManager::generationMessage, ui->logEdit, &QPlainTextEdit::appendPlainText);
  QObject::connect(&getDocument(), &DocumentDataManager::generationErrorOccured, ui->logEdit, &QPlainTextEdit::appendPlainText);
  
  return true;
}
