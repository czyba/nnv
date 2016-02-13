#ifndef _AUTO_VECTOR_HPP
#define _AUTO_VECTOR_HPP

#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <maths.h>

using namespace std;

template <typename T>
class auto_vector {
public:
  auto_vector() : m_size(0), m_capacity(0), m_data(static_cast<T*>(malloc(sizeof(T) * m_capacity))) {
  }

  auto_vector(T* initial_data, size_t size) : m_size(size), m_capacity(next_power_of_two(size)), m_data(static_cast<T*>(malloc(sizeof(T) * m_capacity))) {
    for (size_t i = 0; i < size; i++) {
      m_data[i] = initial_data[i];
    }
  }

  ~auto_vector() {
    free(m_data);
  }

  T const& operator[](size_t index) const {
    return m_data[index];
  }

  T const* data() const {
    return m_data;
  }

  void insertAt(T const e, size_t const index) {
    if (index > m_size) {
      //Out of range
      return;
    }
    //Maximum capacity, resize
    if (m_size == m_capacity) {
      if ((m_capacity << 1) < m_capacity) {
        //Cannot resize
        return;
      }
      //In the off case that m_capacity is 0 set it to 1
      m_capacity = m_capacity ? m_capacity << 1 : 1;
      m_size++;

      //Copy data, inser element and copy rest
      m_data = static_cast<T*>(realloc(m_data, sizeof(T) * m_capacity));
      T* src = m_data + index;
      T* dest = m_data + index + 1;

      for (size_t i = index + 1; i < m_size; i++) {
        *(dest++) = *(src++);
      }

      m_data[index] = e;
      return;
    }

    m_size++;

    T* dest = m_data + index + 1;
    T* src = m_data + index;
    for (size_t i = index + 1; i < m_size; i++) {
      *(dest++) = *(src++);
    }
    m_data[index] = e;
    return;
  }

  void deleteAt(size_t index) {
    if (index > m_size) {
      //Out of range
      return;
    }

    index++;
    for (; index < m_size; index++) {
      m_data[index - 1] = m_data[index];
    }

    m_size--;
    if (next_power_of_two(m_size) < m_capacity) {
      m_capacity >>= 1;
      m_data = static_cast<T*>(realloc(m_data, sizeof(T) * m_capacity));
    }
  }

  template <typename OT>
  friend ostream& operator<<(ostream& os, auto_vector<OT>& vec) {
    os << "vec [ ";
    if (!vec.m_size) {
      os << "]";
      return os;
    }
    os << vec[0];
    for (size_t i = 1; i < vec.m_size; i++) {
      os << ", " << vec[i];
    }
    os << " ]";
    return os;
  }

  size_t size() {
    return m_size;
  }

  size_t capacity() {
    return m_capacity;
  }

private:
  size_t m_size;
  size_t m_capacity;
  T* m_data;
};

#endif