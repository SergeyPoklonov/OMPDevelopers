#include "generationpage.h"
#include "Document/documentdatamanager.h"
#include "ui_generationpage.h"
#include <QFileDialog>
#include <QString>
#include <QTimer> 

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

void GenerationPage::createAndSaveHTML()
{
  QString fileToSaveFilePath = QFileDialog::getSaveFileName(this, "Укажите файл для сохранения", getDocument().getOutputHTMLDefaultFilePath(), "HyperText document (*.html)");
  
  if( fileToSaveFilePath.isEmpty() )
    return;
}

void GenerationPage::startGeneration()
{
  const bool generationOK = getDocument().generateWorkData();
  
  if( generationOK )
  {
    ui->logEdit->appendPlainText("Сбор и анализ данных успешно завершен.");
    
    ui->generateHTMLButton->setEnabled( true );
  }
  else
  {
    ui->logEdit->appendPlainText("Сбор и анализ данных прерван из-за возникших ошибок.");
  }
}

void GenerationPage::initializePage()
{
  QWizardPage::initializePage();
  
  ui->generateHTMLButton->setEnabled( false );
  
  QTimer::singleShot(200, this, &GenerationPage::startGeneration);
}

bool GenerationPage::initialize( DocumentDataManager *doc )
{
  if( !CWizardPageDocumentSupport::initialize( doc ) )
    return false;
  
  QObject::connect(&getDocument(), &DocumentDataManager::generationStepsNumUpdated, ui->progressBar, &QProgressBar::setMaximum);
  QObject::connect(&getDocument(), &DocumentDataManager::generationStepsDone, ui->progressBar, &QProgressBar::setValue);
  QObject::connect(&getDocument(), &DocumentDataManager::generationMessage, ui->logEdit, &QPlainTextEdit::appendPlainText);
  QObject::connect(&getDocument(), &DocumentDataManager::generationErrorOccured, ui->logEdit, &QPlainTextEdit::appendPlainText);
  
  QObject::connect(ui->generateHTMLButton, &QPushButton::clicked, this, &GenerationPage::createAndSaveHTML);
  
  return true;
}
