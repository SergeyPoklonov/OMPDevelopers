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
  
  const bool generationOK = getDocument().generateWorkData();
  
  if( generationOK )
  {
    ui->logEdit->appendPlainText("Сбор и анализ данных успешно завершен.");
  }
  else
  {
    ui->logEdit->appendPlainText("Сбор и анализ данных прерван из-за возникших ошибок.");
  }
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
