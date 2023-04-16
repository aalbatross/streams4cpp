#ifndef INCLUDED_STREAMS4CPP_LISTITERATOR_VIEW_H_
#define INCLUDED_STREAMS4CPP_LISTITERATOR_VIEW_H_
#include "iterator.h"
namespace aalbatross::utils {

template<typename Container,
         typename T = typename Container::value_type>
struct ListIteratorView : public Iterator<T> {
  explicit ListIteratorView(Container &storage)
      : dData_(storage), dBegin_(dData_.begin()), dEnd_(dData_.end()), dCurrent_(dData_.begin()), dLast_((dBegin_ != dEnd_) ? std::optional<T>{*(dData_.begin())} : std::optional<T>{}) {
  }

  ~ListIteratorView() = default;

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

  inline void reset() override {
    dBegin_ = dData_.begin();
    dCurrent_ = dData_.begin();
    dEnd_ = dData_.end();
    if (dBegin_ != dEnd_) {
      dLast_ = *(dData_.begin());
    }
  }

 private:
  Container dData_;
  decltype(dData_.begin()) dBegin_;
  decltype(dData_.end()) dEnd_;
  decltype(dData_.begin()) dCurrent_;
  std::optional<T> dLast_;
};

template<typename Container,
         typename T = typename Container::value_type>
ListIteratorView(Container &) -> ListIteratorView<Container, T>;
}// namespace aalbatross::utils

#endif//INCLUDED_STREAMS4CPP_LISTITERATOR_VIEW_H_
