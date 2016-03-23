#include "developerdatamanager.h"
#include "apputils.h"

void CDeveloperData::clear()
{
  m_Name.clear();
  m_WageRate = 1.0;
}

void CDeveloperListDataManager::clear()
{
  m_DevelopersList.clear();
}

CDeveloperData CDeveloperListDataManager::GetByInd( size_t ind ) const
{
  Q_ASSERT( utils::is_valid_index( m_DevelopersList, ind ) );

  if( !utils::is_valid_index( m_DevelopersList, ind ) )
  {
    return CDeveloperData();
  }

  return m_DevelopersList[ind];
}

CDeveloperData CDeveloperListDataManager::GetByName( QString name ) const
{
  return CDeveloperData();
}
