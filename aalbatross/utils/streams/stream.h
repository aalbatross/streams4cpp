#ifndef INCLUDED_STREAMS4CPP_STREAM_H_
#define INCLUDED_STREAMS4CPP_STREAM_H_
#include "aalbatross/utils/iterators/iterator.h"
#include "aalbatross/utils/iterators/listiterator_view.h"
#include "collector.h"

#include <algorithm>
#include <deque>
#include <list>
#include <optional>
#include <set>
#include <unordered_set>

namespace aalbatross::utils::streams {
/**
 * \class Stream
 * \brief A sequence of elements supporting sequential and parallel aggregate operations.
 *
 * To perform a computation, stream operations are composed into a stream pipeline. A stream pipeline consists of a source (which might be an array, a collection, a generator function, an I/O channel, etc), zero or more intermediate operations (which transform a stream into another stream, such as filter(Predicate)), and a terminal operation (which produces a result or side-effect, such as count() or forEach(Consumer)). Streams are lazy; computation on the source data is only performed when the terminal operation is initiated, and source elements are consumed only as needed.
 * Collections and streams, while bearing some superficial similarities, have different goals. Collections are primarily concerned with the efficient management of, and access to, their elements. By contrast, streams do not provide a means to directly access or manipulate their elements, and are instead concerned with declaratively describing their source and the computational operations which will be performed in aggregate on that source.
 * @tparam T target stream type
 * @tparam S source stream type
 */
template<typename T, typename S = T>
struct Stream {

  /**
   * \class View
   * \brief Creates a view of underlying Stream.
   */
  struct View final {
   private:
    std::vector<S> dContainer_;
    iterators::ListIterator<typename std::vector<S>::iterator> dIterator_;

    std::vector<S> iteratorToContainer(iterators::Iterator<S> &iterator) {
      std::vector<S> result;
      while (iterator.hasNext()) {
        result.emplace_back(iterator.next().value());
      }
      return result;
    }

   public:
    explicit View(iterators::Iterator<S> &iterator) : dContainer_(iteratorToContainer(iterator)), dIterator_(iterators::ListIterator(dContainer_.begin(), dContainer_.end())) {}

    virtual ~View() = default;

    View(View &) = default;
    View(View &&) noexcept = default;

    auto operator=(const View &) -> View & = default;
    auto operator=(View &&) noexcept -> View & = default;

    auto stream() -> Stream<S, S> {
      return Stream<S, S>(dIterator_);
    }
  };

  Stream(iterators::Iterator<S> &source,
         std::function<std::unique_ptr<iterators::Iterator<T>>(iterators::Iterator<S> &)> mapper)
      : dMapper_(std::move(mapper)), dSource_(source) {}

  explicit Stream(iterators::Iterator<S> &source) : dSource_(source) {
    std::function<std::unique_ptr<iterators::Iterator<T>>(iterators::Iterator<S> &)> mapper =
        [](iterators::Iterator<S> &source) {
          std::vector<S> result;
          while (source.hasNext()) {
            result.emplace_back(source.next().value());
          }
          return std::make_unique<iterators::ListIteratorView<std::vector<S>>>(result);
        };
    dMapper_ = mapper;
  }

  /**
   * \fn bool allMatch(std::function<bool(T)> predicate)
   * \brief Returns whether all elements of this stream match the provided predicate. May not evaluate the predicate on all elements if not necessary for determining the result. If the stream is empty then true is returned and the predicate is not evaluated.
   * @param predicate  stateless predicate to apply to elements of this stream
   * @return     true if either all elements of the stream match the provided predicate or the stream is empty, otherwise false
   */
  bool allMatch(std::function<bool(T)> predicate) {
    auto vec = toVector();
    return std::all_of(vec.begin(), vec.end(), predicate);
  }

  /**
   * \fn bool anyMatch(std::function<bool(T)> predicate)
   * \brief Returns whether any elements of this stream match the provided predicate. May not evaluate the predicate on all elements if not necessary for determining the result. If the stream is empty then false is returned and the predicate is not evaluated.
   * @param predicate stateless predicate to apply to elements of this stream
   * @return     true if any elements of the stream match the provided predicate, otherwise false
   */
  bool anyMatch(std::function<bool(T)> predicate) {
    auto vec = toVector();
    return std::any_of(vec.begin(), vec.end(), predicate);
  }

  /**
   * \fn bool noneMatch(std::function<bool(T)> predicate)
   * \brief Returns whether no elements of this stream match the provided predicate. May not evaluate the predicate on all elements if not necessary for determining the result. If the stream is empty then true is returned and the predicate is not evaluated.
   * @param predicate stateless predicate to apply to elements of this stream
   * @return     true if either no elements of the stream match the provided predicate or the stream is empty, otherwise false
   */
  bool noneMatch(std::function<bool(T)> predicate) {
    auto vec = toVector();
    return std::none_of(vec.begin(), vec.end(), predicate);
  }

  /**
   * \fn std::optional<T> head()
   * \brief Returns an Optional describing the first element of this stream, or an empty Optional if the stream is empty. If the stream has no encounter order, then any element may be returned.
   * @return an Optional describing the first element of this stream, or an empty Optional if the stream is empty
   */
  std::optional<T> head() {
    auto vec = toVector();
    return vec.empty() ? std::optional<T>() : std::optional<T>(vec.front());
  }

  /**
   * \fn std::optional<T> tail()
   * \brief Returns an Optional describing the last element of this stream, or an empty Optional if the stream is empty. If the stream has no encounter order, then any element may be returned.
   * @return an Optional describing the last element of this stream, or an empty Optional if the stream is empty
   */
  std::optional<T> tail() {
    auto vec = toVector();
    return vec.empty() ? std::optional<T>() : std::optional<T>(vec.back());
  }

  /**
   * \fn std::optional<T> find(std::function<bool(T)> predicate)
   * \brief Finds the first element of the sequence satisfying a predicate, if any.
   * @param predicate stateless predicate to apply to elements of this stream
   * @return an Optional describing the first element of this stream which matches with this predicate, or an empty Optional if the stream is empty
   */
  std::optional<T> find(std::function<bool(T)> predicate) {
    auto vec = toVector();
    auto iterator = std::find_if(vec.begin(), vec.end(), predicate);
    return iterator != vec.end() ? std::optional<T>(*iterator) : std::optional<T>{};
  }

  /**
   * \fn std::unordered_map<K, std::vector<T>> groupedBy(Discriminator &&discriminator)
   * \brief Partitions this traversable collection into a map of traversable collections according to some discriminator function.
   * @tparam Discriminator It is the type of Function which when applied with element of stream returns Discriminator
   * @param discriminator It is definition of function to identify the key discriminator based on elements of stream
   * @return An unordered map with key as discriminator and a vector of the stream elements.
   */
  template<typename Discriminator>
  auto groupedBy(Discriminator &&discriminator) {
    auto vec = toVector();
    using K = typename std::invoke_result<Discriminator, T>::type;
    std::unordered_map<K, std::vector<T>> output;
    for (auto element : vec) {
      K key = discriminator(element);
      output.try_emplace(key, std::vector<T>());
      output[key].emplace_back(element);
    }
    return output;
  }

  /**
   * \fn Stream<E, S> map(Fun &&mapper)
   * \brief Returns a stream consisting of the results of applying the given function to the elements of this stream.
   * @tparam Fun It is a type of function which can map elements from one type to another.
   * @param mapper is mapping function an object of Fun to transform element of stream from one type to other
   * @return Stream object with composed mapper function.
   */
  template<typename Fun>
  auto map(Fun &&mapper) {
    using E = typename std::invoke_result<Fun, S>::type;
    std::function<std::unique_ptr<iterators::Iterator<E>>(iterators::Iterator<S> &)> newMapper =
        [mapper, this](iterators::Iterator<S> &source) {
          auto inter = dMapper_(source);
          std::vector<E> result;
          while (inter->hasNext()) {
            result.emplace_back(mapper(inter->next().value()));
          }
          return std::make_unique<iterators::ListIteratorView<std::vector<E>>>(result);
        };
    return Stream<E, S>(dSource_, newMapper);
  }

  /**
   * \fn Stream<T, S> filter(std::function<bool(T)> predicate)
   * \brief Selects all elements of this stream which satisfy a predicate.
   * @param predicate stateless predicated which is applied to all elements of stream.
   * @return Stream of filtered elements as per predicate.
   */
  Stream<T, S> filter(std::function<bool(T)> predicate) {
    std::function<std::unique_ptr<iterators::Iterator<T>>(iterators::Iterator<S> &)> newMapper =
        [this, predicate](iterators::Iterator<S> &source) {
          auto inter = dMapper_(source);
          std::vector<T> result;
          while (inter->hasNext()) {
            const auto element = inter->next().value();
            if (predicate(element)) {
              result.emplace_back(element);
            }
          }
          return std::make_unique<iterators::ListIteratorView<std::vector<T>>>(result);
        };
    return Stream<T, S>(dSource_, newMapper);
  }

  /**
   * \fn Stream<T, S> limit(const size_t count)
   * \brief Returns a stream consisting of the elements of this stream, truncated to be no longer than count in length.
   * @param count max size of the stream.
   * @return Stream object returning elements of the provided size.
   */
  Stream<T, S> limit(const size_t count) {
    std::function<std::unique_ptr<iterators::Iterator<T>>(iterators::Iterator<S> &)> newMapper =
        [this, count](iterators::Iterator<S> &source) {
          auto inter = dMapper_(source);
          std::vector<T> result;
          while (inter->hasNext()) {
            if (result.size() < count) {
              result.emplace_back(inter->next().value());
            }
          }
          return std::make_unique<iterators::ListIteratorView<std::vector<T>>>(result);
        };
    return Stream<T, S>(dSource_, newMapper);
  }

  /**
   * \fn Stream<T, S> skip(const size_t count)
   * \brief Returns a stream consisting of the remaining elements of this stream after discarding the first count elements of the stream.
   * @param count the number of leading elements to skip
   * @return new stream with skipped elements from stream.
   */
  Stream<T, S> skip(const size_t count) {
    std::function<std::unique_ptr<iterators::Iterator<T>>(iterators::Iterator<S> &)> newMapper =
        [this, count](iterators::Iterator<S> &source) {
          auto inter = dMapper_(source);
          std::vector<T> result;
          size_t currentCount = 0;
          while (inter->hasNext()) {
            if (currentCount >= count) {
              result.emplace_back(inter->next().value());
            }
            currentCount++;
          }
          return std::make_unique<iterators::ListIteratorView<std::vector<T>>>(result);
        };
    return Stream<T, S>(dSource_, newMapper);
  }

  /**
   * \fn Stream<T, S> sorted(std::function<int(T, T)> comparator)
   * \brief Returns a stream consisting of the elements of this stream, sorted according to comparator.
   * @param comparator  stateless Comparator to be used to compare stream elements
   * @return new stream
   */
  Stream<T, S> sorted(std::function<int(T, T)> comparator) {
    std::function<std::unique_ptr<iterators::Iterator<T>>(iterators::Iterator<S> &)> newMapper =
        [this, comparator](iterators::Iterator<S> &source) {
          auto inter = dMapper_(source);
          std::vector<T> result;
          while (inter->hasNext()) {
            result.emplace_back(inter->next().value());
          }
          std::sort(result.begin(), result.end(), comparator);
          return std::make_unique<iterators::ListIteratorView<std::vector<T>>>(result);
        };
    return Stream<T, S>(dSource_, newMapper);
  }
  /**
   * \fn Stream<T, S> distinct()
   * \brief Returns a stream consisting of the distinct elements (according to element1 == element2) of this stream.
   * @return new stream with unique elements.
   */
  Stream<T, S> distinct() {
    std::function<std::unique_ptr<iterators::Iterator<T>>(iterators::Iterator<S> &)> newMapper =
        [this](iterators::Iterator<S> &source) {
          auto inter = dMapper_(source);
          std::set<T> result;
          while (inter->hasNext()) {
            result.emplace(inter->next().value());
          }
          return std::make_unique<iterators::ListIteratorView<std::set<T>>>(result);
        };
    return Stream<T, S>(dSource_, newMapper);
  }
  /**
   * \fn Stream<T, S> reverse()
   * \brief A list consisting of all elements of this list in reverse order.
   * @return a new stream representing original stream elements in reverse order.
   */
  Stream<T, S> reverse() {
    std::function<std::unique_ptr<iterators::Iterator<T>>(iterators::Iterator<S> &)> newMapper =
        [this](iterators::Iterator<S> &source) {
          auto inter = dMapper_(source);
          std::vector<T> result;
          while (inter->hasNext()) {
            result.emplace_back(inter->next().value());
          }
          std::reverse(result.begin(), result.end());
          return std::make_unique<iterators::ListIteratorView<std::vector<T>>>(result);
        };
    return Stream<T, S>(dSource_, newMapper);
  }

  /**
   * \fn std::optional<T> max()
   * \brief Max of all the elements in this stream.
   * @return Optionally max element if the stream is non empty else empty optional.
   */
  std::optional<T> max() {
    std::vector<T> vec = toVector();
    auto iterator = std::max_element(vec.begin(), vec.end());
    return iterator != vec.end() ? std::optional<T>(*iterator) : std::optional<T>{};
  }

  /**
   * \fn std::optional<T> min()
   * \brief Min of all the elements in this stream.
   * @return Optionally min element if the stream is non empty else empty optional.
   */
  std::optional<T> min() {
    std::vector<T> vec = toVector();
    auto iterator = std::min_element(vec.begin(), vec.end());
    return iterator != vec.end() ? std::optional<T>(*iterator) : std::optional<T>{};
  }

  /**
   * \fn T sum()
   * \brief Sum of all the elements in this stream.
   * @return sum
   */
  T sum() {
    auto sumAccumulator = [](auto x_1, auto y_1) { return x_1 + y_1; };
    return reduce(T{}, sumAccumulator);
  }

  template<typename Supplier, typename Accumulator, typename Finisher>
  auto collect(Collector<Supplier, Accumulator, Finisher> &&collector) {
    std::vector<T> vec = toVector();
    return collector.apply(vec);
  }

  /**
   * \fn T reduce(T identity, std::function<T(T, T)> binaryAccumulator)
   * \brief Performs a reduction on the elements of this stream, using the provided identity value and an associative accumulation function, and returns the reduced value.
   * @param identity the identity value for the accumulating function
   * @param binaryAccumulator an associative, non-interfering, stateless function for combining two values
   * @return the result of the reduction
   */
  T reduce(T identity, std::function<T(T, T)> binaryAccumulator) {
    dSource_.reset();
    auto result = dMapper_(dSource_);
    T output = identity;
    while (result->hasNext()) {
      output = binaryAccumulator(output, result->next().value());
    }
    return output;
  }
  /**
   * \fn size_t count()
   * \brief Returns the count of elements in this stream. This is a special case of a reduction and is equivalent to:
   * @return size of the stream.
   */
  size_t count() {
    dSource_.reset();
    auto result = dMapper_(dSource_);
    size_t count = 0;
    while (result->hasNext()) {
      count++;
    }
    return count;
  }

  /**
   * \fn void forEach(Consumer &&consumer)
   * Performs an action for each element of this stream.
   * @tparam Consumer type of Consumer function a non-interfering action to perform on the elements
   * @param consumer  a non-interfering Consumer action to perform on the elements
   */
  template<typename Consumer>
  void forEach(Consumer &&consumer) {
    dSource_.reset();
    auto result = dMapper_(dSource_);
    while (result->hasNext()) {
      consumer(result->next().value());
    }
  }

  /**
   * \fn std::set<T> toSet()
   * \brief Returns set of input elements.
   * @return stream as set
   */
  auto toSet() { return toSetImpl<std::set<T>>(); }

  /**
   * \fn std::unordered_set<T> toUnorderedSet()
   * \brief Returns unordered set of input elements.
   * @return stream as unordered set
   */
  auto toUnorderedSet() { return toSetImpl<std::unordered_set<T>>(); }

  /**
   * std::vector<T> toVector()
   * \brief Returns vector of input elements.
   * @return stream as std::vector
   */
  auto toVector() { return to<std::vector<T>>(); }

  /**
   * std::list<T> toList()
   * \brief Returns list of input elements.
   * @return stream as std::list
   */
  auto toList() { return to<std::list<T>>(); }

  /**
   * std::deque<T> toDeque()
   * \brief Returns deque of input elements.
   * @return stream as std::deque
   */
  auto toDeque() { return to<std::deque<T>>(); }

  /**
   * \fn StreamView<T, S> view()
   * \brief Creates Stream View which copies internal iterator
   * @return Stream<S>::View of the Stream
   */
  View view() {
    return View(dSource_);
  }

 private:
  std::function<std::unique_ptr<iterators::Iterator<T>>(iterators::Iterator<S> &)> dMapper_;
  iterators::Iterator<S> &dSource_;

  template<typename Container>
  inline auto to() {
    dSource_.reset();
    auto inter = dMapper_(dSource_);
    Container result;
    while (inter->hasNext()) {
      result.emplace_back(inter->next().value());
    }
    return result;
  }

  template<typename Container>
  inline auto toSetImpl() {
    dSource_.reset();
    auto inter = dMapper_(dSource_);
    Container result;
    while (inter->hasNext()) {
      result.emplace(inter->next().value());
    }
    return result;
  }
};
}// namespace aalbatross::utils::streams

#endif// INCLUDED_STREAMS4CPP_STREAM_H_