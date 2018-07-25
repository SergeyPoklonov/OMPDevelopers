#ifndef DEVELOPEREDITOR_H
#define DEVELOPEREDITOR_H

#include <QDialog>
#include <QTextCharFormat>

#include <Document/developerdatamanager.h>

namespace Ui {
class DeveloperEditor;
}

struct ExceptDayData;

class DeveloperEditor : public QDialog
{
  Q_OBJECT

public:
  explicit DeveloperEditor(const CDeveloperData &initialData, QDate initialDate, QWidget *parent = 0);
  explicit DeveloperEditor(QDate initialDate, QWidget *parent = 0);
  ~DeveloperEditor();

  enum EditMode
  {
    notSet = 0,
    insert = 1,
    update = 2
  };

  CDeveloperData getDevData() const { return m_DeveloperData; }
  EditMode getEditMode() const { return m_EditMode; }

public slots:
  void OnOK();
  void onDateSelected();
  void onExceptDayPropertiesChanged();
  void onExceptDayTypeChanged();
  void onSetPeriodClicked();

signals:
  void dataAccepted( DeveloperEditor *editor );
  
private:
  void initializeForm( QDate initialDate );
  
  void initializeCalendarWidget( QDate &initialDate );
  void connectExceptDayCtrl();
  
  void updateCurDayStyle();
  
  QTextCharFormat getDayFormat(const ExceptDayData *dayData);

private:
  Ui::DeveloperEditor *ui;

  CDeveloperData m_DeveloperData;

  EditMode m_EditMode;
  
  bool m_SelectedDayDataUpdating;
};

#endif // DEVELOPEREDITOR_H
