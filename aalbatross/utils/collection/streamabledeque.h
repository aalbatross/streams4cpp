#ifndef INCLUDED_STREAMS4CPP_STREAMEDDEQUE_H
#define INCLUDED_STREAMS4CPP_STREAMEDDEQUE_H

#include "aalbatross/utils/iterators/listiterator.h"
#include "aalbatross/utils/streams/stream.h"
#include "streamablecollection.h"

#include <deque>
#include <memory>

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

  explicit SDeque() : std::deque<S, Allocator>() {}

  explicit SDeque(std::initializer_list<S> init,
                  const Allocator &alloc = Allocator()) : std::deque<S, Allocator>(init, alloc) {}

  SDeque(const SDeque &) = default;
  SDeque(SDeque &&) noexcept = default;

  SDeque &operator=(const SDeque &) = default;
  SDeque &operator=(SDeque &&) noexcept = default;
  ~SDeque() = default;

  streams::Stream<S, S> stream() override {
    return streams::Stream<S, S>(this->begin(), this->end());
  }

  std::shared_ptr<iterators::Iterator<S>> iterator() override {
    return std::make_shared<iterators::ListIteratorView<SDeque>>(*this);
  }
};

template<typename S, typename Allocator = std::allocator<S>>
SDeque(std::initializer_list<S> init, const Allocator &alloc = Allocator()) -> SDeque<S, Allocator>;
}// namespace aalbatross::utils::collection
#endif//INCLUDED_STREAMS4CPP_STREAMEDDEQUE_H
