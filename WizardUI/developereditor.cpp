#include "developereditor.h"
#include "ui_developereditor.h"

#include <QMessageBox>

DeveloperEditor::DeveloperEditor(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DeveloperEditor),
  m_EditMode( EditMode::insert )
{
  ui->setupUi(this);

  QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(OnOK()));
}

DeveloperEditor::DeveloperEditor(const CDeveloperData &initialData, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DeveloperEditor),
  m_DeveloperData( initialData ),
  m_EditMode( EditMode::update )
{
  ui->setupUi(this);

  ui->nameEdit->setText( m_DeveloperData.getName() );
  ui->wageRateEdit->setText( QString::number( m_DeveloperData.getWageRate(), 'f', 2 ) );

  QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(OnOK()));
}

DeveloperEditor::~DeveloperEditor()
{
  delete ui;
}

void DeveloperEditor::OnOK()
{
  QString devName = ui->nameEdit->text();

  QString devWageRateStr = ui->wageRateEdit->text();
          devWageRateStr.replace( ",", "." );

  bool conversionOK = false;
  const double devWageRate = devWageRateStr.toDouble( &conversionOK );

  if( !conversionOK )
  {
    QMessageBox::warning(this, "Данные разработчика", "Некорректно задана ставка разработчика. Введите число больше 0." );
    return;
  }

  m_DeveloperData.setName( devName );
  m_DeveloperData.setWageRate( devWageRate );

  QString dataCheckError;

  if( !m_DeveloperData.isValid( &dataCheckError ) )
  {
    QMessageBox msgBox;
                msgBox.setText( dataCheckError );
                msgBox.exec();

    return;
  }

  emit dataAccepted( this );
}
