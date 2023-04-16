#ifndef INCLUDED_STREAMS4CPP_STREAM_H_
#define INCLUDED_STREAMS4CPP_STREAM_H_
#include "iterator.h"
#include "listiterator_view.h"

#include <deque>
#include <list>
#include <set>
#include <unordered_set>

namespace aalbatross::utils {

template<typename T, typename S = T>
struct Stream {
  Stream(Iterator<S> &source,
         std::function<std::unique_ptr<Iterator<T>>(Iterator<S> &)> mapper)
      : d_mapper(mapper), d_source(source) {}

  explicit Stream(Iterator<S> &source) : d_source(source) {
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

  template<typename Fun>
  auto map(Fun &&mapper) {
    using E = typename std::invoke_result<Fun, S>::type;
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
        [&, predicate](Iterator<S> &source) {
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

  Stream<T, S> skip(const size_t count) {
    std::function<std::unique_ptr<Iterator<T>>(Iterator<S> &)> newMapper =
        [&, count](Iterator<S> &source) {
          auto inter = d_mapper(source);
          std::vector<T> result;
          size_t currentCount = 0;
          while (inter->hasNext()) {
            if (currentCount >= count) {
              result.emplace_back(inter->next());
            }
            currentCount++;
          }
          return std::make_unique<ListIteratorView<std::vector<T>>>(result);
        };
    return Stream<T, S>(d_source, newMapper);
  }

  Stream<T, S> sorted(std::function<int(T, T)> comparator) {
    std::function<std::unique_ptr<Iterator<T>>(Iterator<S> &)> newMapper =
        [&, comparator](Iterator<S> &source) {
          auto inter = d_mapper(source);
          std::vector<T> result;
          while (inter->hasNext()) {
            result.emplace_back(inter->next());
          }
          std::sort(result.begin(), result.end(), comparator);
          return std::make_unique<ListIteratorView<std::vector<T>>>(result);
        };
    return Stream<T, S>(d_source, newMapper);
  }

  Stream<T, S> distinct() {
    std::function<std::unique_ptr<Iterator<T>>(Iterator<S> &)> newMapper =
        [&](Iterator<S> &source) {
          auto inter = d_mapper(source);
          std::set<T> result;
          while (inter->hasNext()) {
            result.emplace(inter->next());
          }
          return std::make_unique<ListIteratorView<std::set<T>>>(result);
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

  size_t count() {
    d_source.reset();
    auto result = d_mapper(d_source);
    size_t count = 0;
    while (result->hasNext()) {
      count++;
    }
    return count;
  }

  template<typename Consumer>
  void forEach(Consumer &&consumer) {
    d_source.reset();
    auto result = d_mapper(d_source);
    while (result->hasNext()) {
      consumer(result->next());
    }
  }

  auto toSet() { return toSetImpl<std::set<T>>(); }

  auto toUnorderedSet() { return toSetImpl<std::unordered_set<T>>(); }

  auto toVector() { return to<std::vector<T>>(); }

  auto toList() { return to<std::list<T>>(); }

  auto toDeque() { return to<std::deque<T>>(); }

 private:
  std::function<std::unique_ptr<Iterator<T>>(Iterator<S> &)> d_mapper;
  Iterator<S> &d_source;

  template<typename Container>
  inline auto to() {
    d_source.reset();
    auto inter = d_mapper(d_source);
    Container result;
    while (inter->hasNext()) {
      result.emplace_back(inter->next());
    }
    return result;
  }

  template<typename Container>
  inline auto toSetImpl() {
    d_source.reset();
    auto inter = d_mapper(d_source);
    Container result;
    while (inter->hasNext()) {
      result.emplace(inter->next());
    }
    return result;
  }
};
}// namespace aalbatross::utils

#endif// INCLUDED_STREAMS4CPP_STREAM_H_
