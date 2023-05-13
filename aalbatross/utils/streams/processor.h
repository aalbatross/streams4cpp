#ifndef INCLUDED_STREAMS4CPP_PROCESSOR_H_
#define INCLUDED_STREAMS4CPP_PROCESSOR_H_

#include "collection/streamabledeque.h"

#include <any>
#include <iostream>
#include <memory>
namespace aalbatross::utils::streams {
/**
 * \class Processor
 * \brief Unbound stream processor base class for all Streaming Processors.
 */
struct Processor {
  virtual ~Processor() = default;

  void setListener(std::shared_ptr<Processor> processor) {
    dListener_ = std::move(processor);
  }

  template<typename T>
  void process(T input) {
    processImpl(std::move(std::any(input)));
  }

  virtual void reset() = 0;

 protected:
  std::shared_ptr<Processor> dListener_;
  virtual void processImpl(const std::any &value) = 0;
};
/**
 * \class MapProcessor
 * \brief Unbound stream processor derived from Processor, applies mapping to incoming streaming inputs.
 * @tparam Mapper
 * @tparam IN
 */
template<typename Mapper, typename IN>
struct MapProcessor : public Processor {
  MapProcessor(Mapper &&mapper) : dMapper_(std::forward<Mapper>(mapper)) {
  }

  ~MapProcessor() override = default;

  void reset() override {}

 protected:
  void processImpl(const std::any &value) override {
    if (dListener_) {
      try {
        const auto &input = std::any_cast<IN>(value);
        dListener_->process(std::move(dMapper_(input)));
      } catch (const std::bad_any_cast &e) {
        std::cout << e.what() << " Expected type:" << value.type().name() << '\n';
      }
    }
  }

 private:
  Mapper dMapper_;
};

/**
 * \class FlatMapProcessor
 * \brief Unbound stream processor derived from Processor, applies mapping to incoming streaming inputs. Note the incoming inputs are of container type.
 * @tparam Mapper
 * @tparam IN
 */
template<typename Mapper, typename IN>
struct FlatMapProcessor : public Processor {
  FlatMapProcessor(Mapper &&mapper) : dMapper_(std::forward<Mapper>(mapper)) {
  }

  ~FlatMapProcessor() override = default;

  void reset() override {}

 protected:
  void processImpl(const std::any &value) override {
    if (dListener_) {
      try {
        const auto &input = std::any_cast<IN>(value);
        for (const auto &element : input) {
          dListener_->process(std::move(dMapper_(element)));
        }
      } catch (const std::bad_any_cast &e) {
        std::cout << e.what() << " Expected type:" << value.type().name() << '\n';
      }
    }
  }

 private:
  Mapper dMapper_;
};

/**
 * \class ConsumerProcessor
 * \brief Unbound stream processor derived from Processor, applies Consumer function to incoming streaming inputs.
 * @tparam Mapper
 * @tparam IN
 */
template<typename Consumer, typename IN>
struct ConsumerProcessor : public Processor {

  ConsumerProcessor(Consumer &&consumer) : dConsumer_(std::forward<Consumer>(consumer)) {
  }

  ~ConsumerProcessor() override = default;

  void reset() override {}

 protected:
  void processImpl(const std::any &value) override {
    const auto &input = std::any_cast<IN>(value);
    dConsumer_(input);
  }

 private:
  Consumer dConsumer_;
};
/**
 * \class FilterProcessor
 * \brief Unbound stream processor derived from Processor, applies Predicate function to incoming streaming inputs to filter the appropriate element from stream.
 * @tparam Mapper
 * @tparam IN
 */
template<typename Predicate, typename IN>
struct FilterProcessor : public Processor {
  FilterProcessor(Predicate &&predicate) : dPredicate_(std::forward<Predicate>(predicate)) {
  }

  ~FilterProcessor() override = default;

  void reset() override {}

 protected:
  void processImpl(const std::any &value) override {
    if (dListener_) {
      try {
        const auto &input = std::any_cast<IN>(value);
        if (dPredicate_(input)) {
          dListener_->process(input);
        }
      } catch (const std::bad_any_cast &e) {
        std::cout << e.what() << " Expected type:" << value.type().name() << '\n';
      }
    }
  }

 private:
  Predicate dPredicate_;
};
/**
 * \class LimitProcessor
 * \brief Unbound stream processor derived from Processor, limits the incoming stream to provided size.
 * @tparam IN
 */
template<typename IN>
struct LimitProcessor : public Processor {
  LimitProcessor(size_t limit) : dLimit_(limit) {}

  ~LimitProcessor() override = default;

  void reset() override {
    dCount_ = 0;
  }

 protected:
  void processImpl(const std::any &value) override {
    if (dListener_) {
      try {
        const auto &input = std::any_cast<IN>(value);
        if (dCount_ >= dLimit_) {
          return;
        }
        if (dCount_ < dLimit_) {
          dListener_->process(std::move(input));
        }
        dCount_++;
      } catch (const std::bad_any_cast &e) {
        std::cout << e.what() << " Expected type:" << value.type().name() << '\n';
      }
    }
  }

 private:
  size_t dCount_ = 0;
  size_t dLimit_;
};
/**
 * \class SkipProcessor
 * \brief Unbound stream processor derived from Processor, skips elements from incoming stream.
 * @tparam IN
 */
template<typename IN>
struct SkipProcessor : public Processor {
  SkipProcessor(size_t skipLimit) : dLimit_(skipLimit) {}

  ~SkipProcessor() override = default;

  void reset() override {
    dCount_ = 0;
  }

 protected:
  void processImpl(const std::any &value) override {
    if (dListener_) {
      try {
        const auto &input = std::any_cast<IN>(value);
        if (dCount_ >= dLimit_) {
          dListener_->process(input);
        } else {
          dCount_++;
        }
      } catch (const std::bad_any_cast &e) {
        std::cout << e.what() << " Expected type:" << value.type().name() << '\n';
      }
    }
  }

 private:
  size_t dCount_ = 0;
  size_t dLimit_;
};
/**
 * \class SlidingWindowProcessor
 * \brief Unbound stream processor derived from Processor, groups elements into sliding windows of provided size.
 * @tparam IN
 */
template<typename IN>
struct SlidingWindowProcessor : public Processor {
  SlidingWindowProcessor(size_t windowSize) : dWindowSize_(windowSize) {}

  ~SlidingWindowProcessor() override = default;

  void reset() override {
    dElements_.clear();
  }

 protected:
  void processImpl(const std::any &value) override {
    if (dListener_) {
      try {
        const auto &input = std::any_cast<IN>(value);
        dElements_.emplace_back(std::move(input));
        if (dElements_.size() == dWindowSize_) {
          dListener_->process(dElements_);
          //remove first element to maintain size less than window
          dElements_.pop_front();
        }
      } catch (const std::bad_any_cast &e) {
        std::cout << e.what() << " Expected type:" << value.type().name() << '\n';
      }
    }
  }

 private:
  collection::SDeque<IN> dElements_;
  size_t dWindowSize_;
};
/**
 * \class FixedWindowProcessor
 * \brief Unbound stream processor derived from Processor, groups elements into fixed size windows of provided size.
 * @tparam IN
 */
template<typename IN>
struct FixedWindowProcessor : public Processor {
  FixedWindowProcessor(size_t windowSize) : dWindowSize_(windowSize) {}

  ~FixedWindowProcessor() override = default;

  void reset() override {
    dElements_.clear();
  }

 protected:
  void processImpl(const std::any &value) override {
    if (dListener_) {
      try {
        const auto &input = std::any_cast<IN>(value);
        dElements_.emplace_back(std::move(input));
        if (dElements_.size() == dWindowSize_) {
          dListener_->process(dElements_);
          //remove first element to maintain size less than window
          dElements_.clear();
        }
      } catch (const std::bad_any_cast &e) {
        std::cout << e.what() << " Expected type:" << value.type().name() << '\n';
      }
    }
  }

 private:
  collection::SDeque<IN> dElements_;
  size_t dWindowSize_;
};

}// namespace aalbatross::utils::streams
#endif//INCLUDED_STREAMS4CPP_PROCESSOR_H_
