#ifndef INCLUDED_STREAMS4CPP__LISTITERATOR_H_
#define INCLUDED_STREAMS4CPP__LISTITERATOR_H_

#include "iterator.h"

namespace aalbatross::util {

template<typename Iter,
         typename T = typename std::iterator_traits<Iter>::value_type>
struct ListIterator : public Iterator<T> {
  ListIterator(Iter &&begin, Iter &&end)
      : d_begin(begin), d_end(end), d_current(begin), d_last(*begin) {}

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

};// namespace aalbatross::util

#endif// INCLUDED_STREAMS4CPP__LISTITERATOR_H_
