#ifndef DEVELOPEREDITOR_H
#define DEVELOPEREDITOR_H

#include <QDialog>

#include <Document/developerdatamanager.h>

namespace Ui {
class DeveloperEditor;
}

class DeveloperEditor : public QDialog
{
  Q_OBJECT

public:
  explicit DeveloperEditor(const CDeveloperData &initialData, QWidget *parent = 0);
  explicit DeveloperEditor(QWidget *parent = 0);
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

signals:
  void dataAccepted( DeveloperEditor *editor );

private:
  Ui::DeveloperEditor *ui;

  CDeveloperData m_DeveloperData;

  EditMode m_EditMode;
};

#endif // DEVELOPEREDITOR_H
