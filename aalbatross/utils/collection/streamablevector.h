#ifndef INCLUDED_STREAMS4CPP_STREAMVECTOR_H
#define INCLUDED_STREAMS4CPP_STREAMVECTOR_H

#include "aalbatross/utils/iterators/listiterator.h"
#include "aalbatross/utils/streams/stream.h"
#include "streamablecollection.h"

#include <vector>

namespace aalbatross::utils::collection {
/**
 * \class SVector
 * \brief Streamable Vector class extends STL Vector and implements StreamableCollection interface.
 *
 * Streamable Vector can be used to transform Vector using map/reduce style operations on Stream.
 * @tparam S element type
 * @tparam Allocator
 */
template<typename S, typename Allocator = std::allocator<S>>
struct SVector final : public std::vector<S, Allocator>, public SCollection<S> {

  explicit SVector() : std::vector<S, Allocator>(), dIterator_(std::make_unique<iterators::ListIterator<typename std::vector<S>::iterator>>(this->begin(), this->end())) {}

  explicit SVector(std::initializer_list<S> init,
                   const Allocator &alloc = Allocator()) : std::vector<S, Allocator>(init, alloc), dIterator_(std::make_unique<iterators::ListIterator<typename std::vector<S>::iterator>>(this->begin(), this->end())) {}
  SVector(const SVector &) = default;
  SVector(SVector &&) noexcept = default;

  SVector &operator=(const SVector &) = default;
  SVector &operator=(SVector &&) noexcept = default;
  ~SVector() = default;

  streams::Stream<S, S> stream() override {
    dIterator_ = std::move(std::make_unique<iterators::ListIterator<typename std::vector<S>::iterator>>(this->begin(), this->end()));
    return streams::Stream<S, S>(*dIterator_);
  }

  iterators::Iterator<S> &iterator() override {
    dIterator_ = std::move(std::make_unique<iterators::ListIterator<typename std::vector<S>::iterator>>(this->begin(), this->end()));
    return *dIterator_;
  }

 private:
  std::unique_ptr<iterators::Iterator<S>> dIterator_;
};

template<typename S, typename Allocator = std::allocator<S>>
SVector(std::initializer_list<S> init, const Allocator &alloc = Allocator()) -> SVector<S, Allocator>;
}// namespace aalbatross::utils::collection
#endif//INCLUDED_STREAMS4CPP_STREAMVECTOR_H
