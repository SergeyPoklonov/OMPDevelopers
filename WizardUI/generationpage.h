#ifndef GENERATIONPAGE_H
#define GENERATIONPAGE_H

#include "wizardpagedocumentsupport.h"

#include <QWizardPage>

namespace Ui {
class GenerationPage;
}

class GenerationPage : public QWizardPage
                     , public CWizardPageDocumentSupport
{
  Q_OBJECT
  
public:
  explicit GenerationPage(QWidget *parent = 0);
  ~GenerationPage();
  
  virtual bool initialize( DocumentDataManager *doc ) override;
  
  virtual void initializePage() override;
  
private:
  Ui::GenerationPage *ui;
};

#endif // GENERATIONPAGE_H
