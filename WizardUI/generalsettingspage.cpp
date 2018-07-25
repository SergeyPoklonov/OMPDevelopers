#include "developereditor.h"
#include "generalsettingspage.h"
#include "ui_generalsettingspage.h"

#include <Document/developerdatamanager.h>
#include <Document/documentdatamanager.h>

#include <QMessageBox>
#include <QFileDialog>

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

bool GeneralSettingsPage::validatePage()
{
  getDocument().generalSettings().setRedmineAuthKey( ui->authKeyEdit->text() );
  getDocument().generalSettings().setRedmineURL( ui->redmineURLEdit->text() );
  getDocument().generalSettings().setGitWeb( ui->gitWebEdit->text() );
  
  QString errStr;
  if( !getDocument().generalSettings().isVaild(&errStr) )
  {
    QMessageBox::warning(this, "Продолжение работы", errStr);
    return false;
  }
  
  if( !getDocument().SaveGeneralSettings() )
  {
    QMessageBox::StandardButton userChoise = QMessageBox::question(this, "Продолжение работы", "Невозможно сохранить файл настроек. Продолжить работу без сохранения?");
    if( userChoise != QMessageBox::Yes )
      return false;
  }

  return QWizardPage::validatePage();
}

bool GeneralSettingsPage::initialize( DocumentDataManager *doc )
{
  if( !CWizardPageDocumentSupport::initialize( doc ) )
    return false;

  ui->developrsList->setModel( &(getDocument().generalSettings().getDevelopersManager()) );

  QObject::connect(ui->developrsList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &GeneralSettingsPage::developersListSelectionChange);
  
  ui->developrsList->sortByColumn(0, Qt::AscendingOrder);

  QObject::connect(ui->developerAdd, SIGNAL(clicked()), this, SLOT(developerAddClick()));
  QObject::connect(ui->developerEdit, SIGNAL(clicked()), this, SLOT(developerEditClick()));
  QObject::connect(ui->developerDel, SIGNAL(clicked()), this, SLOT(developerDelClick()));

  QObject::connect(ui->developrsList, &QTableView::doubleClicked, this, &GeneralSettingsPage::developerEditClick);
  
  QObject::connect(ui->repositoryChooseButton, &QPushButton::clicked, this, &GeneralSettingsPage::gitRepositoryChoose);
  
  ui->redmineURLEdit->setText( getDocument().generalSettings().getRedmineURL() );
  ui->authKeyEdit->setText( getDocument().generalSettings().getRedmineAuthKey() );
  
  developersListSelectionChange();
  setGitRepositoryString( getDocument().generalSettings().getGitPath() );
  
  ui->gitWebEdit->setText( getDocument().generalSettings().getGitWeb() );

  return true;
}

void GeneralSettingsPage::setGitRepositoryString(QString gitRepository)
{
  ui->repositoryEdit->setText( gitRepository );
}

void GeneralSettingsPage::gitRepositoryChoose()
{ 
  const QString gitDir = QFileDialog::getExistingDirectory(this, "Расположение репозитория Git", getDocument().generalSettings().getGitPath() );
  
  if( !gitDir.isEmpty() )
  {
    getDocument().generalSettings().setGitPath( gitDir );
    setGitRepositoryString( getDocument().generalSettings().getGitPath() );
  }
}

void GeneralSettingsPage::developerAddClick()
{
  DeveloperEditor editor(QDate::currentDate(),this);

  auto conn = QObject::connect(&editor, &DeveloperEditor::dataAccepted, this, &GeneralSettingsPage::developerDataEditorDone);

  editor.exec();
}

int GeneralSettingsPage::getDevListCurSelIndex()
{
  QModelIndexList selIndexes = ui->developrsList->selectionModel()->selectedRows();

  if( selIndexes.size() )
  {
    return selIndexes[0].row();
  }

  return -1;
}

void GeneralSettingsPage::developerDelClick()
{
  const int curSelInd = getDevListCurSelIndex();

  if( curSelInd >= 0 )
  {
    if( QMessageBox::Yes != QMessageBox::question(this, "Удаление разработчика", "Вы действительно хотите удалить разработчика?" ) )
      return;

    QString errStr;

    if( !getDocument().generalSettings().getDevelopersManager().DeleteData( curSelInd, &errStr ) )
    {
      QMessageBox::warning(this, "Удаление разработчика", QString("Невозможно удалить разработчика: %1.").arg( errStr ) );
      return;
    }

    ui->developrsList->setFocus( Qt::OtherFocusReason );
  }
}

void GeneralSettingsPage::developerEditClick()
{
  const int selInd = getDevListCurSelIndex();

  if( selInd >= 0 )
  {
    CDeveloperData devData = getDocument().generalSettings().getDevelopersManager().GetDataByInd( selInd );

    DeveloperEditor editor(devData, QDate::currentDate(), this);

    auto conn = QObject::connect(&editor, &DeveloperEditor::dataAccepted, this, &GeneralSettingsPage::developerDataEditorDone);

    editor.exec();
  }
}

void GeneralSettingsPage::developersListSelectionChange()
{
  const bool buttonsEnabled = getDevListCurSelIndex() >= 0;

  ui->developerEdit->setEnabled( buttonsEnabled );
  ui->developerDel->setEnabled( buttonsEnabled );
}

void GeneralSettingsPage::developerDataEditorDone( DeveloperEditor *editor )
{
  const int curSelInd = getDevListCurSelIndex();

  if( editor->getEditMode() == DeveloperEditor::insert )
  {
    CDeveloperData devData = editor->getDevData();

    QString errStr;

    bool devCreatedOK = false;

    devCreatedOK = getDocument().generalSettings().getDevelopersManager().AddData( devData, &errStr );

    if( !devCreatedOK )
    {
      QMessageBox::warning(editor, "Создание разработчика", QString("Невозможно создать разработчика: %1.").arg( errStr ) );
      return;
    }

    editor->accept();

    ui->developrsList->setFocus( Qt::PopupFocusReason );
  }

  if( editor->getEditMode() == DeveloperEditor::update )
  {

    Q_ASSERT( curSelInd >= 0 );

    if( curSelInd < 0 )
    {
      return;
    }

    CDeveloperData devData = editor->getDevData();

    QString errStr;

    if( !getDocument().generalSettings().getDevelopersManager().SetDataByInd( devData, curSelInd, &errStr ) )
    {
      QMessageBox::warning(editor, "Редактирование разработчика", QString("Невозможно изменить данные разработчика: %1.").arg( errStr ) );
      return;
    }

    editor->accept();

    ui->developrsList->setFocus( Qt::PopupFocusReason );
  }
}
