#ifndef INCLUDED_STREAMS4CPP_ITERATOR_H
#define INCLUDED_STREAMS4CPP_ITERATOR_H

#include "functional"
namespace aalbatross::utils {
template<typename T>
struct Iterator {
  virtual ~Iterator() = default;

  virtual bool hasNext() = 0;

  virtual T next() = 0;

  virtual void reset() = 0;

  inline void forEachRemaining(std::function<void(T)> consumer) {
    while (hasNext()) {
      consumer(next());
    }
  }
};
}// namespace aalbatross::utils

#endif// INCLUDED_STREAMS4CPP_ITERATOR_H
