#ifndef APPUTILS_H
#define APPUTILS_H

#include <vector>

namespace utils
{
  // is_valid_index
  bool is_valid_index(size_t arraySize, size_t ind)
  {
    return ind < arraySize;
  }

  bool is_valid_index(size_t arraySize, long ind)
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


  // is_first_index
  bool is_first_index(size_t arraySize, size_t ind)
  {
    return !ind && is_valid_index(arraySize, ind);
  }

  bool is_first_index(size_t arraySize, long ind)
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
  bool is_last_index(size_t arraySize, size_t ind)
  {
    return is_valid_index(arraySize, ind) && ((ind + 1) == arraySize);
  }

  bool is_last_index(size_t arraySize, long ind)
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

#endif // APPUTILS_H
