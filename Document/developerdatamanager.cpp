#include "developerdatamanager.h"
#include "apputils.h"
#include <algorithm>

void CDeveloperData::clear()
{
  m_Name.clear();
  m_WageRate = 1.0;
}

void CDeveloperListDataManager::clear()
{
  m_DevelopersList.clear();
}

CDeveloperData CDeveloperListDataManager::GetDataByInd( size_t ind ) const
{
  Q_ASSERT( utils::is_valid_index( m_DevelopersList, ind ) );

  if( !utils::is_valid_index( m_DevelopersList, ind ) )
  {
    return CDeveloperData();
  }

  return m_DevelopersList[ind];
}

long CDeveloperListDataManager::GetIndByName( QString name ) const
{
  auto findedIt = std::find_if(m_DevelopersList.begin(), m_DevelopersList.end(), [name]( const CDeveloperData &data )
  {
    return data.getName() == name;
  });

  if( findedIt == m_DevelopersList.end() )
    return -1;

  return std::distance( m_DevelopersList.begin(), findedIt );
}

CDeveloperData CDeveloperListDataManager::GetDataByName( QString name ) const
{
  auto findedIt = std::find_if(m_DevelopersList.begin(), m_DevelopersList.end(), [name]( const CDeveloperData &data )
  {
    return data.getName() == name;
  });

  if( findedIt == m_DevelopersList.end() )
    return CDeveloperData();

  return *findedIt;
}

void CDeveloperListDataManager::SortData( std::function<bool(const CDeveloperData &f, const CDeveloperData &s)> sortPredicate )
{
  std::sort(m_DevelopersList.begin(), m_DevelopersList.end(), sortPredicate);
}

bool CDeveloperListDataManager::SetDataByInd( CDeveloperData newData, size_t ind )
{
  Q_ASSERT( utils::is_valid_index( m_DevelopersList, ind ) );

  if( !utils::is_valid_index( m_DevelopersList, ind ) )
  {
    return false;
  }

  m_DevelopersList[ind] = newData;

  return true;
}
