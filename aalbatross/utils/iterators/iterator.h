#ifndef INCLUDED_STREAMS4CPP_ITERATOR_H
#define INCLUDED_STREAMS4CPP_ITERATOR_H

#include "functional"
namespace aalbatross::utils::iterators {
/**
 * \class Iterator
 * \brief Interface to traverse a bound or unbound source of data.
 * @tparam T
 */

template<typename T>
struct Iterator {
  virtual ~Iterator() = default;

  /**
   * \fn bool hasNext()
   * \brief check if source or container has more element.
   * @return true if element exist else false
   */
  virtual bool hasNext() = 0;
  /**
   * \fn T next()
   * \brief next element
   * @return element
   */
  virtual std::optional<T> next() = 0;

  /**
   * \fn void reset()
   * \brief reset the source to start from beginning again.
   */
  virtual void reset() = 0;

  /**
   * \fn void forEachRemaining(std::function<void(T)> consumer)
   * \brief iterate over each remaining element and apply consumer on them.
   * @param consumer consumer function on every element of container until element exists.
   */
  inline void forEachRemaining(std::function<void(T)> consumer) {
    while (hasNext()) {
      consumer(next());
    }
  }
};
}// namespace aalbatross::utils::iterators

#endif// INCLUDED_STREAMS4CPP_ITERATOR_H
