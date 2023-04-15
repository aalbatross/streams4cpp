#ifndef STREAMS4CPP__STREAM_H_
#define STREAMS4CPP__STREAM_H_
#include "iterator.h"
#include "listiterator_view.h"
namespace aalbatross::util {

template<typename T, typename S = T>
struct Stream {
  Stream(Iterator<S> &source,
         std::function<std::unique_ptr<Iterator<T>>(Iterator<S> &)> mapper)
      : d_source(source), d_mapper(mapper) {}

  Stream(Iterator<S> &source) : d_source(source) {
    std::function<std::unique_ptr<Iterator<T>>(Iterator<S> &)> x =
        [](Iterator<S> &source) {
          std::vector<S> x;
          while (source.hasNext()) {
            x.emplace_back(source.next());
          }
          return std::make_unique<ListIteratorView<std::vector<S>>>(x);
        };
    d_mapper = x;
  }

  template<typename E, typename Fun>
  Stream<E, S> map(Fun &&mapper) {
    std::function<std::unique_ptr<Iterator<E>>(Iterator<S> &)> newMapper =
        [&](Iterator<S> &source) {
          auto inter = d_mapper(source);
          std::vector<E> result;
          while (inter->hasNext()) {
            result.emplace_back(mapper(inter->next()));
          }
          return std::make_unique<ListIteratorView<std::vector<E>>>(result);
        };
    return Stream<E, S>(d_source, newMapper);
  }

  Stream<T, S> filter(std::function<bool(T)> predicate) {
    std::function<std::unique_ptr<Iterator<T>>(Iterator<S> &)> newMapper =
        [&](Iterator<S> &source) {
          auto inter = d_mapper(source);
          std::vector<T> result;
          while (inter->hasNext()) {
            const auto x = inter->next();
            if (predicate(x)) {
              result.emplace_back(x);
            }
          }
          return std::make_unique<ListIteratorView<std::vector<T>>>(result);
        };
    return Stream<T, S>(d_source, newMapper);
  }

  Stream<T, S> limit(const size_t count) {
    std::function<std::unique_ptr<Iterator<T>>(Iterator<S> &)> newMapper =
        [&, count](Iterator<S> &source) {
          auto inter = d_mapper(source);
          std::vector<T> result;
          while (inter->hasNext()) {
            if (result.size() < count) {
              result.emplace_back(inter->next());
            }
          }
          return std::make_unique<ListIteratorView<std::vector<T>>>(result);
        };
    return Stream<T, S>(d_source, newMapper);
  }

  T reduce(T identity, std::function<T(T, T)> binaryAccumulator) {
    d_source.reset();
    auto result = d_mapper(d_source);
    T output = identity;
    while (result->hasNext()) {
      output = binaryAccumulator(output, result->next());
    }
    return output;
  }

  template<typename Consumer>
  void forEach(Consumer &&consumer) {
    d_source.reset();
    auto result = d_mapper(d_source);
    while (result->hasNext()) {
      consumer(result->next());
    }
  }

 private:
  std::function<std::unique_ptr<Iterator<T>>(Iterator<S> &)> d_mapper;
  Iterator<S> &d_source;
};
};// namespace aalbatross::util

#endif// STREAMS4CPP__STREAM_H_
