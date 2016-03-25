#ifndef WIZARDMAIN_H
#define WIZARDMAIN_H

#include <QWizard>

namespace Ui {
class WizardMain;
}

class WizardMain : public QWizard
{
    Q_OBJECT

public:
    explicit WizardMain(QWidget *parent = 0);
    ~WizardMain();

public:
  bool Initialize();

private:
    Ui::WizardMain *ui;
};

#endif // WIZARDMAIN_H
