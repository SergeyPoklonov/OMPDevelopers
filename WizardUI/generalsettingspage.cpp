#include "developereditor.h"
#include "generalsettingspage.h"
#include "ui_generalsettingspage.h"

#include <Document/developerdatamanager.h>
#include <Document/documentdatamanager.h>

#include <QMessageBox>

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
  if( !getDocument().SaveGeneralSettings() )
  {
    QMessageBox::critical(this, "Продолжение работы", "Невозможно сохранить файл настроек.");
    return false;
  }

  return QWizardPage::validatePage();
}

bool GeneralSettingsPage::initialize( DocumentDataManager *doc )
{
  if( !CWizardPageDocumentSupport::initialize( doc ) )
    return false;

  ui->developrsList->setModel( &(getDocument().getDevelopersManager()) );

  QObject::connect(ui->developrsList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &GeneralSettingsPage::developersListSelectionChange);
  
  ui->developrsList->sortByColumn(0, Qt::AscendingOrder);

  developersListSelectionChange();

  QObject::connect(ui->developerAdd, SIGNAL(clicked()), this, SLOT(developerAddClick()));
  QObject::connect(ui->developerEdit, SIGNAL(clicked()), this, SLOT(developerEditClick()));
  QObject::connect(ui->developerDel, SIGNAL(clicked()), this, SLOT(developerDelClick()));

  QObject::connect(ui->developrsList, &QTableView::doubleClicked, this, &GeneralSettingsPage::developerEditClick);

  return true;
}

void GeneralSettingsPage::developerAddClick()
{
  DeveloperEditor editor(this);

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

    if( !getDocument().getDevelopersManager().DeleteData( curSelInd, &errStr ) )
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
    CDeveloperData devData = getDocument().getDevelopersManager().GetDataByInd( selInd );

    DeveloperEditor editor(devData, this);

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

    devCreatedOK = getDocument().getDevelopersManager().AddData( devData, &errStr );

    /*if( curSelInd < 0 )
    {
      devCreatedOK = getDocument().getDevelopersManager().AddData( devData, &errStr );
    }
    else
    {
      devCreatedOK = getDocument().getDevelopersManager().InsertData( devData, curSelInd, &errStr );
    }*/

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

    if( !getDocument().getDevelopersManager().SetDataByInd( devData, curSelInd, &errStr ) )
    {
      QMessageBox::warning(editor, "Редактирование разработчика", QString("Невозможно изменить данные разработчика: %1.").arg( errStr ) );
      return;
    }

    editor->accept();

    ui->developrsList->setFocus( Qt::PopupFocusReason );
  }
}
