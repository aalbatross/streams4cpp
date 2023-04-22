#ifndef INCLUDED_STREAMS4CPP_LISTITERATOR_VIEW_H_
#define INCLUDED_STREAMS4CPP_LISTITERATOR_VIEW_H_
#include "iterator.h"
namespace aalbatross::utils::iterators {
/**
 * \class ListIteratorView
 * \brief Concrete Class implementation of Iterator to iterate over a sequential list of elements.
 *
 * The difference between ListIterator and ListIteratorView is, that ListIteratorView class maintains the copy of data locally hence the presence of source container in scope is not essential.
 *
 * @tparam Container Sequential Container type
 * @tparam T type of element
 */
template<typename Container,
         typename T = typename Container::value_type>
struct ListIteratorView : public Iterator<T> {
  explicit ListIteratorView(Container &storage)
      : dData_(storage), dBegin_(dData_.begin()), dEnd_(dData_.end()), dCurrent_(dData_.begin()), dLast_((dBegin_ != dEnd_) ? std::optional<T>{*(dData_.begin())} : std::optional<T>{}) {
  }

  ListIteratorView(ListIteratorView &) = default;
  ListIteratorView(ListIteratorView &&) noexcept = default;

  auto operator=(const ListIteratorView &) -> ListIteratorView & = default;
  auto operator=(ListIteratorView &&) noexcept -> ListIteratorView & = default;

  ~ListIteratorView() = default;

  /**
   * \fn bool hasNext()
   * \brief check if list has more element.
   * @return true if element exist else false
   */
  inline auto hasNext() -> bool override {
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
}// namespace aalbatross::utils::iterators

#endif//INCLUDED_STREAMS4CPP_LISTITERATOR_VIEW_H_
