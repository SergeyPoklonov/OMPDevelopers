#ifndef GENERALSETTINGSPAGE_H
#define GENERALSETTINGSPAGE_H

#include "wizardpagedocumentsupport.h"

#include <QWizardPage>
#include <QModelIndexList>

namespace Ui {
class GeneralSettingsPage;
}

class DeveloperEditor;

class GeneralSettingsPage : public QWizardPage
                          , public CWizardPageDocumentSupport
{
  Q_OBJECT

public:
  explicit GeneralSettingsPage(QWidget *parent = 0);
  ~GeneralSettingsPage();

  virtual bool initialize( DocumentDataManager *doc ) override;

  virtual bool validatePage() override;

private slots:
  void developerAddClick();
  void developerEditClick();
  void developerDelClick();

  void developersListSelectionChange();

  void developerDataEditorDone( DeveloperEditor *editor );

private:
  int getDevListCurSelIndex();

private:
  Ui::GeneralSettingsPage *ui;
};

#endif // GENERALSETTINGSPAGE_H
