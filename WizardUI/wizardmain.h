#ifndef WIZARDMAIN_H
#define WIZARDMAIN_H

#include <QWizard>

namespace Ui {
class WizardMain;
}

class DocumentDataManager;

class WizardMain : public QWizard
{
    Q_OBJECT

public:
    explicit WizardMain(QWidget *parent = 0);
    ~WizardMain();
  
public:
  enum class PAGEID
  {
    GENERAL_SETTINGS = 1,
    LOCAL_SETTINGS = 2,
    GENERATION = 3
  };

public:
  bool Initialize();

private:
    Ui::WizardMain *ui;

    DocumentDataManager *m_Document;
};

#endif // WIZARDMAIN_H
