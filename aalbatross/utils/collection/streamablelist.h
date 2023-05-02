#ifndef INCLUDED_STREAMS4CPP_STREAMEDLIST_H
#define INCLUDED_STREAMS4CPP_STREAMEDLIST_H

#include "aalbatross/utils/iterators/listiterator.h"
#include "streamablecollection.h"

#include <list>
#include <memory>
namespace aalbatross::utils::collection {

/**
 * \class SList
 * \brief Streamable List class extends STL List and implements StreamableCollection interface.
 *
 * Streamable List can be used to transform List using map/reduce style operations on Stream.
 * @tparam S element type
 * @tparam Allocator
 */
template<typename S, typename Allocator = std::allocator<S>>
struct SList final : public std::list<S, Allocator>, public SCollection<S> {

  explicit SList() : std::list<S, Allocator>() {}

  explicit SList(std::initializer_list<S> init,
                 const Allocator &alloc = Allocator()) : std::list<S, Allocator>(init, alloc) {}
  SList(const SList &) = default;
  SList(SList &&) noexcept = default;

  SList &operator=(const SList &) = default;
  SList &operator=(SList &&) noexcept = default;
  ~SList() = default;

  streams::Stream<S, S> stream() override {
    return streams::Stream<S, S>(this->begin(), this->end());
  }

  std::shared_ptr<iterators::Iterator<S>> iterator() override {
    return std::make_shared<iterators::ListIteratorView<SList>>(*this);
  }
};

template<typename S, typename Allocator = std::allocator<S>>
SList(std::initializer_list<S> init, const Allocator &alloc = Allocator()) -> SList<S, Allocator>;
}// namespace aalbatross::utils::collection
#endif//INCLUDED_STREAMS4CPP_STREAMEDLIST_H
