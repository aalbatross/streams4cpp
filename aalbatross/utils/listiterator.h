#ifndef INCLUDED_STREAMS4CPP_LISTITERATOR_H
#define INCLUDED_STREAMS4CPP_LISTITERATOR_H

#include "iterator.h"

#include <optional>

namespace aalbatross::utils {

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

  inline bool hasNext() override {
    bool hasMore = dCurrent_ != dEnd_;
    if (hasMore) {
      dLast_ = *dCurrent_;
      std::advance(dCurrent_, 1);
    }
    return hasMore;
  }

  inline T next() override {
    return dLast_.value();
  }

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

}// namespace aalbatross::utils

#endif// INCLUDED_STREAMS4CPP_LISTITERATOR_H
