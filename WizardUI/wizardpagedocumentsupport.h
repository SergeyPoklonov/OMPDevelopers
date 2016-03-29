#ifndef WIZARDPAGE_H
#define WIZARDPAGE_H

class DocumentDataManager;

class CWizardPageDocumentSupport
{
public:
  CWizardPageDocumentSupport();

  void setDocument( DocumentDataManager *doc );

  DocumentDataManager& getDocument();

  bool isValidDocumentPtr();

private:
  DocumentDataManager *m_Doc;
};

#endif // WIZARDPAGE_H
