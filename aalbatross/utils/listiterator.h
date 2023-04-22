#ifndef INCLUDED_STREAMS4CPP_LISTITERATOR_H
#define INCLUDED_STREAMS4CPP_LISTITERATOR_H

#include "iterator.h"

#include <optional>

namespace aalbatross::utils::iterators {
/**
 * \class ListIterator
 * \brief Concrete Class implementation of Iterator to iterate over a sequential list of elements.
 *
 * The class uses the reference of the source container, ensure the source container is in the scope of call
 * @tparam Iter iterator of container
 * @tparam T element of container
 */
template<typename Iter,
         typename T = typename std::iterator_traits<Iter>::value_type>
struct ListIterator : public Iterator<T> {
  ListIterator(Iter &&begin, Iter &&end)
      : dBegin_(begin), dEnd_(end), dCurrent_(begin), dLast_(dBegin_ != dEnd_ ? std::optional<T>{*dBegin_} : std::optional<T>{}) {
  }

  ListIterator(ListIterator &) = default;
  ListIterator(ListIterator &&) noexcept = default;

  ListIterator &operator=(const ListIterator &) = default;
  ListIterator &operator=(ListIterator &&) noexcept = default;
  ~ListIterator() = default;

  /**
   * \fn bool hasNext()
   * \brief check if list has more element.
   * @return true if element exist else false
   */
  inline bool hasNext() override {
    bool hasMore = dCurrent_ != dEnd_;
    if (hasMore) {
      dLast_ = *dCurrent_;
      std::advance(dCurrent_, 1);
    }
    return hasMore;
  }
  /**
   * \fn T next()
   * \brief next element in the list
   * @return element
   */
  inline T next() override {
    return dLast_.value();
  }

  /**
   * \fn void reset()
   * \brief reset the source to start from beginning again.
   */
  inline void reset() override { dCurrent_ = dBegin_; }

 private:
  Iter dBegin_;
  Iter dEnd_;
  Iter dCurrent_;
  std::optional<T> dLast_;
};

template<typename Iter,
         typename T = typename std::iterator_traits<Iter>::value_type>
ListIterator(Iter &&, Iter &&) -> ListIterator<Iter, T>;

}// namespace aalbatross::utils::iterators

#endif// INCLUDED_STREAMS4CPP_LISTITERATOR_H
