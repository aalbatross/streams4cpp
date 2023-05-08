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

  template<typename Mapper, typename E = typename std::invoke_result<Mapper, T>::type>
  UBStream<E, T, BASE> map(Mapper &&mapper) {
    std::vector<std::shared_ptr<Processor>> copy(dProcessors_);
    copy.emplace_back(std::make_shared<MapProcessor<Mapper, T>>(std::forward<Mapper>(mapper)));
    return UBStream<E, T, BASE>(copy, dSourceData_);
  }

  template<typename Mapper>
  auto flatMap(Mapper &&mapper) {
    std::vector<std::shared_ptr<Processor>> copy(dProcessors_);
    copy.emplace_back(std::make_shared<FlatMapProcessor<Mapper, T>>(std::forward<Mapper>(mapper)));
    return UBStream<typename std::invoke_result<Mapper, typename T::value_type>::type, T, BASE>(copy, dSourceData_);
  }

  template<typename Predicate>
  UBStream<T, T, BASE> filter(Predicate &&predicate) {
    std::vector<std::shared_ptr<Processor>> copy(dProcessors_);
    copy.emplace_back(std::make_shared<FilterProcessor<Predicate, T>>(std::forward<Predicate>(predicate)));
    return UBStream<T, T, BASE>(copy, dSourceData_);
  }

  UBStream<T, T, BASE> limit(size_t limit) {
    std::vector<std::shared_ptr<Processor>> copy(dProcessors_);
    copy.emplace_back(std::make_shared<LimitProcessor<T>>(limit));
    return UBStream<T, T, BASE>(copy, dSourceData_);
  }

  UBStream<T, T, BASE> skip(size_t skipCount) {
    std::vector<std::shared_ptr<Processor>> copy(dProcessors_);
    copy.emplace_back(std::make_shared<SkipProcessor<T>>(skipCount));
    return UBStream<T, T, BASE>(copy, dSourceData_);
  }

  UBStream<std::deque<T>, T, BASE> sliding(size_t windowSize) {
    std::vector<std::shared_ptr<Processor>> copy(dProcessors_);
    copy.emplace_back(std::make_shared<SlidingWindowProcessor<T>>(windowSize));
    return UBStream<std::deque<T>, T, BASE>(copy, dSourceData_);
  }

  UBStream<std::deque<T>, T, BASE> fixed(size_t windowSize) {
    std::vector<std::shared_ptr<Processor>> copy(dProcessors_);
    copy.emplace_back(std::make_shared<FixedWindowProcessor<T>>(windowSize));
    return UBStream<std::deque<T>, T, BASE>(copy, dSourceData_);
  }

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

  std::vector<T> toVector() {
    std::vector<T> result;
    auto consumer = [&result](auto element) { result.emplace_back(element); };
    forEach(consumer);
    return result;
  }

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
