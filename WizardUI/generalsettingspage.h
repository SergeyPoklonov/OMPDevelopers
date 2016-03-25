#ifndef GENERALSETTINGSPAGE_H
#define GENERALSETTINGSPAGE_H

#include <QWizardPage>
#include <QModelIndexList>

namespace Ui {
class GeneralSettingsPage;
}

class GeneralSettingsPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit GeneralSettingsPage(QWidget *parent = 0);
  ~GeneralSettingsPage();

  bool Initialize();

private:
  bool InitializeDevelopersList();

private:
  Ui::GeneralSettingsPage *ui;
};

#endif // GENERALSETTINGSPAGE_H
