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

public slots:
      void onDevelopersListSelChanged(const QModelIndexList &indexes);

private:
  Ui::GeneralSettingsPage *ui;
};

#endif // GENERALSETTINGSPAGE_H
