#ifndef INCLUDED_STREAMS4CPP_STREAMEDDEQUE_H
#define INCLUDED_STREAMS4CPP_STREAMEDDEQUE_H

#include "aalbatross/utils/iterators/listiterator.h"
#include "aalbatross/utils/streams/stream.h"
#include "streamablecollection.h"

#include <deque>

namespace aalbatross::utils::collection {
/**
 * \class SDeque
 * \brief Streamable Deque class extends STL Deque and implements StreamableCollection interface.
 *
 * Streamable Deque can be used to transform queue using map/reduce style operations on Stream.
 * @tparam S element type
 * @tparam Allocator
 */
template<typename S, typename Allocator = std::allocator<S>>
struct SDeque final : public std::deque<S, Allocator>, public SCollection<S> {

  SDeque() : std::deque<S, Allocator>(), dIterator_(std::make_unique<iterators::ListIterator<typename std::deque<S>::iterator>>(this->begin(), this->end())) {}

  SDeque(std::initializer_list<S> init,
         const Allocator &alloc = Allocator()) : std::deque<S, Allocator>(init, alloc), dIterator_(std::make_unique<iterators::ListIterator<typename std::deque<S>::iterator>>(this->begin(), this->end())) {}

  ~SDeque() = default;

  streams::Stream<S, S> stream() override {
    dIterator_ = std::move(std::make_unique<iterators::ListIterator<typename std::deque<S>::iterator>>(this->begin(), this->end()));
    return streams::Stream<S, S>(*dIterator_);
  }

  iterators::Iterator<S> &iterator() override {
    dIterator_ = std::move(std::make_unique<iterators::ListIterator<typename std::deque<S>::iterator>>(this->begin(), this->end()));
    return *dIterator_;
  }

 private:
  std::unique_ptr<iterators::Iterator<S>> dIterator_;
};

template<typename S, typename Allocator = std::allocator<S>>
SDeque(std::initializer_list<S> init, const Allocator &alloc = Allocator()) -> SDeque<S, Allocator>;
}// namespace aalbatross::utils::collection
#endif//INCLUDED_STREAMS4CPP_STREAMEDDEQUE_H
