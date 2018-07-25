#ifndef APPUTILS_H
#define APPUTILS_H

#include <QComboBox>
#include <vector>

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

#endif // APPUTILS_H
