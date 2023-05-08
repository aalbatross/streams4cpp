#ifndef INCLUDED_STREAMS4CPP_UB_STREAM_H_
#define INCLUDED_STREAMS4CPP_UB_STREAM_H_
#include "aalbatross/utils/iterators/listiterator.h"
#include "collector.h"
#include "processor.h"

#include <iostream>
#include <utility>
#include <vector>
namespace aalbatross::utils::streams {
template<typename T, typename S = T, typename BASE = T>
struct UBStream {
  template<typename Iter>
  UBStream(Iter &&begin, Iter &&end) : dSourceData_(std::make_shared<iterators::ListIterator<Iter>>(std::forward<Iter>(begin), std::forward<Iter>(end))), dProcessors_(std::vector<std::shared_ptr<Processor>>()) {}

  UBStream(std::vector<std::shared_ptr<Processor>> processorsList, std::shared_ptr<iterators::Iterator<BASE>> sourceData) : dProcessors_(std::move(processorsList)), dSourceData_(sourceData) {}

  /**
   * \fn UBStream<E, S> map(Fun &&mapper)
   * \brief Returns a stream consisting of the results of applying the given function to the elements of this stream.
   * @tparam Fun It is a type of function which can map elements from one type to another.
   * @param mapper is mapping function an object of Fun to transform element of stream from one type to other
   * @return Stream object with composed mapper function.
   */
  template<typename Mapper, typename E = typename std::invoke_result<Mapper, T>::type>
  UBStream<E, T, BASE> map(Mapper &&mapper) {
    std::vector<std::shared_ptr<Processor>> copy(dProcessors_);
    copy.emplace_back(std::make_shared<MapProcessor<Mapper, T>>(std::forward<Mapper>(mapper)));
    return UBStream<E, T, BASE>(copy, dSourceData_);
  }
  /**
   * \fn auto flatten(Mapper &&mapper)
   * \brief Returns a stream consisting of the results of replacing each element of this stream with the contents of a mapped stream produced by applying the provided mapping function to each element. Each mapped stream is closed after its contents have been placed into this stream. (If a mapped stream is null an empty stream is used, instead.)
   * @tparam Mapper
   * @param mapper
   * @return a new flatten stream
   */
  template<typename Mapper>
  auto flatten(Mapper &&mapper) {
    std::vector<std::shared_ptr<Processor>> copy(dProcessors_);
    copy.emplace_back(std::make_shared<FlatMapProcessor<Mapper, T>>(std::forward<Mapper>(mapper)));
    return UBStream<typename std::invoke_result<Mapper, typename T::value_type>::type, T, BASE>(copy, dSourceData_);
  }

  /**
   * \fn UBStream<T, S> filter(std::function<bool(T)> predicate)
   * \brief Selects all elements of this stream which satisfy a predicate.
   * @param predicate stateless predicated which is applied to all elements of stream.
   * @return Stream of filtered elements as per predicate.
   */
  template<typename Predicate>
  UBStream<T, T, BASE> filter(Predicate &&predicate) {
    std::vector<std::shared_ptr<Processor>> copy(dProcessors_);
    copy.emplace_back(std::make_shared<FilterProcessor<Predicate, T>>(std::forward<Predicate>(predicate)));
    return UBStream<T, T, BASE>(copy, dSourceData_);
  }
  /**
   * \fn UBStream<T, T, BASE> limit(size_t limit)
   * \brief Returns a stream consisting of the elements of this stream, truncated to be no longer than maxSize in length.
   * @param limit
   * @return the new stream
   */
  UBStream<T, T, BASE> limit(size_t limit) {
    std::vector<std::shared_ptr<Processor>> copy(dProcessors_);
    copy.emplace_back(std::make_shared<LimitProcessor<T>>(limit));
    return UBStream<T, T, BASE>(copy, dSourceData_);
  }
  /**
   * \fn UBStream<T, T, BASE> skip(size_t skipCount)
   * \brief Returns a stream consisting of the remaining elements of this stream after discarding the first n elements of the stream. If this stream contains fewer than n elements then an empty stream will be returned.
   * @param skipCount
   * @return a new stream
   */
  UBStream<T, T, BASE> skip(size_t skipCount) {
    std::vector<std::shared_ptr<Processor>> copy(dProcessors_);
    copy.emplace_back(std::make_shared<SkipProcessor<T>>(skipCount));
    return UBStream<T, T, BASE>(copy, dSourceData_);
  }
  /**
   * \fn UBStream<std::deque<T>, T, BASE> sliding(size_t windowSize)
   * \brief Groups elements in fixed size blocks by passing a "sliding window" over them (as opposed to partitioning them, as is done in grouped.) The "sliding window" step is set to one.
   * @param windowSize
   * @return a new stream of containers
   */
  UBStream<std::deque<T>, T, BASE> sliding(size_t windowSize) {
    std::vector<std::shared_ptr<Processor>> copy(dProcessors_);
    copy.emplace_back(std::make_shared<SlidingWindowProcessor<T>>(windowSize));
    return UBStream<std::deque<T>, T, BASE>(copy, dSourceData_);
  }
  /**
   * \fn UBStream<std::deque<T>, T, BASE> fixed(size_t windowSize)
   * \brief Groups elements in fixed size blocks by passing a "tumbling window" over them (as opposed to partitioning them, as is done in grouped.) The "tumbling window" step is set to one.
   * @param windowSize
   * @return a new stream of containers
   */
  UBStream<std::deque<T>, T, BASE> fixed(size_t windowSize) {
    std::vector<std::shared_ptr<Processor>> copy(dProcessors_);
    copy.emplace_back(std::make_shared<FixedWindowProcessor<T>>(windowSize));
    return UBStream<std::deque<T>, T, BASE>(copy, dSourceData_);
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
   * \fn void forEach(Consumer &&consumer)
   * \brief Performs an action for each element of this stream.This is a terminal operation.
   * @tparam Consumer
   * @param consumer
   */
  template<typename Consumer>
  void forEach(Consumer &&consumer) {
    dProcessors_.emplace_back(std::make_shared<ConsumerProcessor<Consumer, T>>(std::forward<Consumer>(consumer)));

    for (size_t i = 0; i < dProcessors_.size() - 1; i++) {
      if (i + 1 <= dProcessors_.size() - 1) {
        dProcessors_[i]->setListener(dProcessors_[i + 1]);
        dProcessors_[i]->reset();
      }
    }
    //reset last processor
    if (!dProcessors_.empty()) {
      dProcessors_.back()->reset();
    }

    auto processor = dProcessors_.front();
    dSourceData_->reset();
    while (dSourceData_->hasNext()) {
      processor->process(dSourceData_->next().value());
    }
    dProcessors_.pop_back();
  }

  /**
   *
   * @return
   */
  std::vector<T> toVector() {
    std::vector<T> result;
    auto consumer = [&result](auto element) { result.emplace_back(element); };
    forEach(consumer);
    return result;
  }

  /**
   * \fn auto collect(Collector<Supplier, Accumulator, Finisher> &&collector)
   * \brief Performs a mutable reduction operation on the elements of this stream. A mutable reduction is one in which the reduced value is a mutable result container, such as an ArrayList, and elements are incorporated by updating the state of the result rather than by replacing the result. This produces a result equivalent to:
   * @tparam Supplier
   * @tparam Accumulator
   * @tparam Finisher
   * @param collector
   * @return result from the collector
   */
  template<typename Supplier, typename Accumulator, typename Finisher>
  auto collect(Collector<Supplier, Accumulator, Finisher> &&collector) {
    std::vector<T> vec = toVector();
    return collector.apply(vec);
  }

 private:
  std::vector<std::shared_ptr<Processor>> dProcessors_;
  std::shared_ptr<iterators::Iterator<BASE>> dSourceData_;
};
}// namespace aalbatross::utils::streams

#endif//INCLUDED_STREAMS4CPP_UB_STREAM_H_
