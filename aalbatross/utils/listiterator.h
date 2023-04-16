#ifndef INCLUDED_STREAMS4CPP_LISTITERATOR_H
#define INCLUDED_STREAMS4CPP_LISTITERATOR_H

#include "iterator.h"

namespace aalbatross::utils {

template<typename Iter,
         typename T = typename std::iterator_traits<Iter>::value_type>
struct ListIterator : public Iterator<T> {
  ListIterator(Iter &&begin, Iter &&end)
      : d_begin(begin), d_end(end), d_current(begin) {
    if (d_begin != d_end)
      d_last = *d_begin;
  }

  ~ListIterator() = default;

  inline bool hasNext() override {
    bool hasMore = d_current != d_end;
    if (hasMore) {
      d_last = *d_current;
      std::advance(d_current, 1);
    }
    return hasMore;
  }

  inline T next() override {
    return d_last;
  }

  inline void reset() override { d_current = d_begin; }

 private:
  Iter d_begin;
  Iter d_end;
  Iter d_current;
  T d_last;
};

template<typename Iter,
         typename T = typename std::iterator_traits<Iter>::value_type>
ListIterator(Iter &&, Iter &&) -> ListIterator<Iter, T>;

}// namespace aalbatross::utils

#endif// INCLUDED_STREAMS4CPP_LISTITERATOR_H
