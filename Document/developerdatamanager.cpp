#include "developerdatamanager.h"
#include "apputils.h"
#include <algorithm>

void CDeveloperData::clear()
{
  m_Name.clear();
  m_WageRate = 1.0;
}

bool CDeveloperData::isValid(QString *errStr) const
{
  if( m_Name.isEmpty() )
  {
    if( errStr )
      *errStr = "Не задано имя разработчика";

    return false;
  }

  if( m_WageRate <= 0.0 )
  {
    if( errStr )
      *errStr = "Неверная ставка, ставка разработчика должна быть больше 0";

    return false;
  }

  return true;
}

void CDeveloperData::ReadFromXML(QDomElement &parentElement)
{
  m_Name = parentElement.attribute( "name" );
  m_WageRate = parentElement.attribute( "wr" ).toDouble();
}

void CDeveloperData::WriteToXML(QDomElement &parentElement)
{
  parentElement.setAttribute( "name", m_Name );
  parentElement.setAttribute( "wr", m_WageRate );
}

////////////////////////////////////////////////////////////////////////////////
CDeveloperListDataManager::CDeveloperListDataManager( QObject * parent )
  : QAbstractTableModel( parent )
{
}

CDeveloperListDataManager::CDeveloperListDataManager( const CDeveloperListDataManager &src, QObject * parent )
  : QAbstractTableModel( parent )
{
  m_DevelopersList = src.m_DevelopersList;
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

void CDeveloperListDataManager::LoadFromXML(QDomElement &parentElement)
{
  beginResetModel();

  clear();

  QDomDocument doc = parentElement.ownerDocument();

  QDomElement baseElement = parentElement.firstChildElement( "DevelopersList" );

  if( !baseElement.isNull() )
  {
    size_t devQty = baseElement.attribute( "Qty" ).toUInt();

    for(size_t i = 0; i < devQty; i++)
    {
      QDomElement developerElement = baseElement.firstChildElement( QString("Developer%1").arg(i+1) );

      if( !developerElement.isNull() )
      {
        CDeveloperData devData;

        devData.ReadFromXML( developerElement );

        if( devData.isValid() )
          m_DevelopersList.push_back( devData );
      }
    }
  }

  endResetModel();
}

void CDeveloperListDataManager::WriteToXML(QDomElement &parentElement)
{
  QDomDocument doc = parentElement.ownerDocument();

  QDomElement baseElement = doc.createElement("DevelopersList");
  parentElement.appendChild( baseElement );

  baseElement.setAttribute("Qty", m_DevelopersList.size());
  for(size_t i = 0; i < m_DevelopersList.size(); i++)
  {
    QDomElement developerElement = doc.createElement( QString("Developer%1").arg(i+1) );
    baseElement.appendChild( developerElement );

    CDeveloperData &devData = m_DevelopersList[i];
    devData.WriteToXML( developerElement );
  }
}

void CDeveloperListDataManager::SortData( std::function<bool(const CDeveloperData &f, const CDeveloperData &s)> sortPredicate )
{
  if( m_DevelopersList.size() )
  {
    std::sort(m_DevelopersList.begin(), m_DevelopersList.end(), sortPredicate);

    emit dataChanged( createIndex(0, COLNUM_NAME), createIndex(m_DevelopersList.size() - 1, COLNUM_WR) );
  }
}

bool CDeveloperListDataManager::CheckIfNameAlreadyExists(QString name, size_t ignoreInd)
{
  for(size_t i = 0; i < m_DevelopersList.size(); i++)
  {
    if( ignoreInd != ULONG_MAX && i == ignoreInd )
      continue;

    if( m_DevelopersList[i].getName() == name )
    {
      return true;
    }
  }

  return false;
}

bool CDeveloperListDataManager::InsertData( CDeveloperData newData, size_t ind, QString *errStr )
{
  if( !newData.isValid(errStr) )
    return false;

  if( CheckIfNameAlreadyExists( newData.getName() ) )
  {
    if( errStr )
      *errStr = "Разработчик с таким именем уже существует";

    return false;
  }

  if( utils::is_valid_index( m_DevelopersList, ind ) )
  {
    beginInsertRows(QModelIndex(), ind, ind);

    m_DevelopersList.insert( m_DevelopersList.begin() + ind, newData );
  }
  else
  {
    beginInsertRows(QModelIndex(), m_DevelopersList.size(), m_DevelopersList.size());

    m_DevelopersList.push_back( newData );
  }

  endInsertRows();

  return true;
}

bool CDeveloperListDataManager::AddData( CDeveloperData newData, QString *errStr )
{
  return InsertData( newData, ULONG_MAX, errStr );
}

bool CDeveloperListDataManager::DeleteData(size_t ind, QString *errStr)
{
  if( !utils::is_valid_index( m_DevelopersList, ind ) )
  {
    if( errStr )
      *errStr = "Неверный индекс разработчика";

    return false;
  }

  beginRemoveRows(QModelIndex(), ind, ind);

  m_DevelopersList.erase( m_DevelopersList.begin() + ind );

  endRemoveRows();

  return true;
}

bool CDeveloperListDataManager::SetDataByInd( CDeveloperData newData, size_t ind, QString *errStr )
{
  if( !newData.isValid(errStr) )
    return false;

  Q_ASSERT( utils::is_valid_index( m_DevelopersList, ind ) );

  if( !utils::is_valid_index( m_DevelopersList, ind ) )
  {
    if( errStr )
      *errStr = "Неверный индекс разработчика";

    return false;
  }

  if( CheckIfNameAlreadyExists(newData.getName(), ind) )
  {
    if( errStr )
      *errStr = "Разработчик с таким именем уже существует";

    return false;
  }

  m_DevelopersList[ind] = newData;

  emit dataChanged( createIndex(ind, COLNUM_NAME), createIndex(ind, COLNUM_WR) );

  return true;
}

int CDeveloperListDataManager::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_DevelopersList.size();
}

int CDeveloperListDataManager::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return COLQTY;
}

QVariant CDeveloperListDataManager::data(const QModelIndex &index, int role) const
{
  if( !index.isValid() )
    return QVariant();

  if( role != Qt::DisplayRole )
    return QVariant();

  const int modelRow = index.row();

  if( !utils::is_valid_index(m_DevelopersList, modelRow) )
    return QVariant();

  const int modelCol = index.column();

  if( !utils::is_valid_index( (size_t)COLQTY, modelCol ) )
    return QVariant();

  const CDeveloperData &devData = m_DevelopersList[modelRow];

  switch( modelCol )
  {
    case COLNUM_NAME: return QVariant::fromValue( devData.getName() );
    case COLNUM_WR: return QVariant::fromValue( devData.getWageRate() );
  }

  return QVariant();
}

QVariant CDeveloperListDataManager::headerData(int section, Qt::Orientation orientation, int role) const
{
  if( orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch( section )
    {
      case COLNUM_NAME: return QVariant::fromValue( QString("Имя") );
      case COLNUM_WR: return QVariant::fromValue( QString("Ставка") );
    }

    return QVariant();
  }

  return QAbstractTableModel::headerData(section, orientation, role);
}

void CDeveloperListDataManager::sort(int column, Qt::SortOrder order/* = Qt::AscendingOrder*/)
{
  switch( column )
  {
    case COLNUM_NAME:
    {
      SortData( [order](const CDeveloperData &f, const CDeveloperData &s)->bool
      {
        if( order == Qt::AscendingOrder )
          return f.getName() < s.getName();

        if( order == Qt::DescendingOrder )
          return f.getName() > s.getName();

        return false;
      });
    }
    break;

    case COLNUM_WR:
    {
      SortData( [order](const CDeveloperData &f, const CDeveloperData &s)->bool
      {
        if( order == Qt::AscendingOrder )
          return f.getWageRate() < s.getWageRate();

        if( order == Qt::DescendingOrder )
          return f.getWageRate() > s.getWageRate();

        return false;
      });
    }
    break;
  }
}
