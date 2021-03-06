#ifndef APPUTILS_H
#define APPUTILS_H

#include <QComboBox>
#include <vector>
#include <set>

class pair1st_getter
{
public:
  template< typename _K, typename _T >
  inline const _K& operator()( const std::pair< _K, _T > &it )
  {
    return it.first;
  }

  template< typename _K, typename _T >
  inline _K& operator()( std::pair< _K, _T > &it )
  {
    return it.first;
  }
};

class pair2nd_getter
{
public:
  template< typename _K, typename _T >
  inline const _T& operator()( const std::pair< _K, _T > &it )
  {
    return it.second;
  }

  template< typename _K, typename _T >
  inline _T& operator()( std::pair< _K, _T > &it )
  {
    return it.second;
  }
};

#define SET2VEC(s,v)  {(v).clear();(v).resize((s).size());std::copy(std::begin(s),std::end(s),(v).begin());static_assert( sizeof( std::remove_reference<decltype(v)>::type::value_type ) >= sizeof( std::remove_reference<decltype(s)>::type::value_type ) , "Possible loose of data" );}
#define MAP2SET1(m,s) {(s).clear();std::transform((m).begin(),(m).end(),std::inserter((s),std::begin(s)),pair1st_getter());static_assert( sizeof( std::remove_reference<decltype(s)>::type::value_type ) >= sizeof( std::remove_reference<decltype(m)>::type::key_type ) , "Possible loose of data" );}

namespace utils
{
  // is_valid_index
  inline bool is_valid_index(size_t arraySize, size_t ind)
  {
    return ind < arraySize;
  }

  inline bool is_valid_index(size_t arraySize, long ind)
  {
    return ind >= 0 && is_valid_index(arraySize, (size_t) ind);
  }

  inline bool is_valid_index(size_t arraySize, int ind)
  {
    return ind >= 0 && is_valid_index(arraySize, (size_t) ind);
  }

  template <class T>
  bool is_valid_index(const std::vector<T> &vec, size_t ind)
  {
    return is_valid_index( vec.size(), ind );
  }

  template <class T>
  bool is_valid_index(const std::vector<T> &vec, long ind)
  {
    return is_valid_index( vec.size(), ind );
  }

  template <class T>
  bool is_valid_index(const std::vector<T> &vec, int ind)
  {
    return is_valid_index( vec.size(), ind );
  }


  // is_first_index
  inline bool is_first_index(size_t arraySize, size_t ind)
  {
    return !ind && is_valid_index(arraySize, ind);
  }

  inline bool is_first_index(size_t arraySize, long ind)
  {
    return ind >= 0 && is_first_index(arraySize, (size_t)ind);
  }

  template <class T>
  bool is_first_index(const std::vector<T> &vec, size_t ind)
  {
    return is_first_index( vec.size(), ind );
  }

  template <class T>
  bool is_first_index(const std::vector<T> &vec, long ind)
  {
    return is_first_index( vec.size(), ind );
  }

  // is_last_index
  inline bool is_last_index(size_t arraySize, size_t ind)
  {
    return is_valid_index(arraySize, ind) && ((ind + 1) == arraySize);
  }

  inline bool is_last_index(size_t arraySize, long ind)
  {
    return ind >= 0 && is_last_index(arraySize, (size_t)ind);
  }

  template <class T>
  bool is_last_index(const std::vector<T> &vec, size_t ind)
  {
    return is_last_index( vec.size(), ind );
  }

  template <class T>
  bool is_last_index(const std::vector<T> &vec, long ind)
  {
    return is_last_index( vec.size(), ind );
  }
}

/////////////////////////////////////////////////////////////////

// ComboBox selection
static bool selectComboItemByData(QComboBox *cb, int data)
{
  for(int i = 0; i < cb->count(); i++)
  {
    if( cb->itemData(i) == data )
    {
      cb->setCurrentIndex( i );
      return true;
    }
  }
  
  return false;
}

static int getComboCurSelData(QComboBox *cb)
{
  return cb->itemData( cb->currentIndex() ).toInt();
}

static QString valuesToString(const std::vector<long> &values, QString separator)
{
  QStringList qsList;
  
  for(int i = 0; i < values.size(); i++)
  {
    qsList.append( QString().setNum( values[i] ) );
  }
  
  return qsList.join( separator );
}

static QString valuesToString(const std::set<long> &values, QString separator)
{
  std::vector<long> vec;
  SET2VEC(values, vec);
  return valuesToString(vec, separator);
}

#endif // APPUTILS_H
