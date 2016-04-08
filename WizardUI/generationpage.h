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
  
  virtual bool isComplete() const override;
  
public slots:
  void createAndSaveHTML();
  void startGeneration();
  
private:
  Ui::GenerationPage *ui;
  
  bool m_GenerationDone;
};

#endif // GENERATIONPAGE_H
