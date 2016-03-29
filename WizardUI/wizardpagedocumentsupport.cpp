#include "wizardpagedocumentsupport.h"
#include <Document/documentdatamanager.h>


CWizardPageDocumentSupport::CWizardPageDocumentSupport()
{
  m_Doc = nullptr;
}

void CWizardPageDocumentSupport::setDocument( DocumentDataManager *doc )
{
  m_Doc = doc;
}

DocumentDataManager& CWizardPageDocumentSupport::getDocument()
{
  Q_ASSERT( isValidDocumentPtr() );

  return *m_Doc;
}

bool CWizardPageDocumentSupport::isValidDocumentPtr()
{
  return m_Doc != nullptr;
}
