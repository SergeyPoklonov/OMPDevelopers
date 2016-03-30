#ifndef WIZARDPAGE_H
#define WIZARDPAGE_H

class DocumentDataManager;

class CWizardPageDocumentSupport
{
public:
  CWizardPageDocumentSupport();

  virtual bool initialize( DocumentDataManager *doc );

  DocumentDataManager& getDocument();

  bool isValidDocumentPtr();

protected:
  void setDocument( DocumentDataManager *doc );

private:
  DocumentDataManager *m_Doc;
};

#endif // WIZARDPAGE_H
