#include "generationpage.h"
#include "Document/documentdatamanager.h"
#include "ui_generationpage.h"
#include <QFileDialog>
#include <QMessageBox>
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
  //QString fileToSaveFilePath = QFileDialog::getSaveFileName(this, "Укажите файл для сохранения", getDocument().getOutputHTMLDefaultFilePath(), "HyperText document (*.html)");

//  if( fileToSaveFilePath.isEmpty() )
//    return;

//  if( !getDocument().devStatistic().creatHTMLDataFile( fileToSaveFilePath ) )
//  {
//    QMessageBox::warning(this, "Сохранение данных в html", "Ошибка при сохранении данных.");
//  }

  QString dirToSaveFilePath = QFileDialog::getExistingDirectory(this, "Укажите директорию для сохранения", getDocument().getOutputHTMLDefaultFilePath());

  if( dirToSaveFilePath.isEmpty() )
    return;
  
  if( !getDocument().devStatistic().creatHTMLDataFiles( dirToSaveFilePath ) )
  {
    QMessageBox::warning(this, "Сохранение данных в html", "Ошибка при сохранении данных.");
  }


}

void GenerationPage::startGeneration()
{
  const bool generationOK = getDocument().devStatistic().generateWorkData();
  
  if( generationOK )
  {
    ui->logEdit->appendPlainText("Сбор и анализ данных успешно завершен.");
    
    ui->generateHTMLButton->setEnabled( true );
  }
  else
  {
    ui->logEdit->appendPlainText("Сбор и анализ данных прерван из-за возникших ошибок.");
  }
  
  m_GenerationDone = true;
  
  emit completeChanged();
}

bool GenerationPage::isComplete() const
{
  return m_GenerationDone;
}

void GenerationPage::initializePage()
{
  QWizardPage::initializePage();
  
  ui->progressBar->setValue(0);
  ui->generateHTMLButton->setEnabled( false );
  
  m_GenerationDone = false;
  
  emit completeChanged();
  
  QTimer::singleShot(200, this, &GenerationPage::startGeneration);
}

bool GenerationPage::initialize( DocumentDataManager *doc )
{
  if( !CWizardPageDocumentSupport::initialize( doc ) )
    return false;
   
  QObject::connect(&getDocument().devStatistic(), &DevStatisticsDocument::generationStepsNumUpdated, ui->progressBar, &QProgressBar::setMaximum);
  QObject::connect(&getDocument().devStatistic(), &DevStatisticsDocument::generationStepsDone, ui->progressBar, &QProgressBar::setValue);
  QObject::connect(&getDocument().devStatistic(), &DevStatisticsDocument::generationMessage, ui->logEdit, &QPlainTextEdit::appendPlainText);
  QObject::connect(&getDocument().devStatistic(), &DevStatisticsDocument::generationErrorOccured, ui->logEdit, &QPlainTextEdit::appendPlainText);
  
  QObject::connect(ui->generateHTMLButton, &QPushButton::clicked, this, &GenerationPage::createAndSaveHTML);
  
  return true;
}
