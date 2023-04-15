#ifndef INCLUDED_STREAMS4CPP__ITERATOR_H_
#define INCLUDED_STREAMS4CPP__ITERATOR_H_

#include <functional>
namespace aalbatross::util {
template <typename T> struct Iterator {
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
} // namespace aalbatross::util

#endif // INCLUDED_STREAMS4CPP__ITERATOR_H_
