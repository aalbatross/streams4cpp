#ifndef INCLUDED_STREAMS4CPP_LISTITERATOR_VIEW_H_
#define INCLUDED_STREAMS4CPP_LISTITERATOR_VIEW_H_
#include "iterator.h"
namespace aalbatross::utils {

template<typename Container,
         typename T = typename Container::value_type>
struct ListIteratorView : public Iterator<T> {
  explicit ListIteratorView(Container &storage)
      : d_data(storage) {
    d_begin = d_data.begin();
    d_end = d_data.end();
    d_current = d_data.begin();
    if (d_begin != d_end)
      d_last = *(d_data.begin());
  }

  ~ListIteratorView() = default;

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

  inline void reset() override {
    d_begin = d_data.begin();
    d_current = d_data.begin();
    d_end = d_data.end();
    if (d_begin != d_end)
      d_last = *(d_data.begin());
  }

 private:
  Container d_data;
  decltype(d_data.begin()) d_begin;
  decltype(d_data.end()) d_end;
  decltype(d_data.begin()) d_current;
  T d_last;
};

template<typename Container,
         typename T = typename Container::value_type>
ListIteratorView(Container &) -> ListIteratorView<Container, T>;
}// namespace aalbatross::utils

#endif//INCLUDED_STREAMS4CPP_LISTITERATOR_VIEW_H_
