#ifndef INCLUDED_STREAMS4CPP_STREAMEDSET_H
#define INCLUDED_STREAMS4CPP_STREAMEDSET_H
#include "aalbatross/utils/iterators/listiterator.h"
#include "streamablecollection.h"

#include <functional>
#include <memory>
#include <set>
namespace aalbatross::utils::collection {
/**
 * \class SSet
 * \brief Streamable Set class extends STL Set and implements StreamableCollection interface.
 *
 * Streamable Set can be used to transform Set using map/reduce style operations on Stream.
 * @tparam Key
 * @tparam Compare
 * @tparam Allocator
 */
template<
    typename Key,
    typename Compare = std::less<Key>,
    typename Allocator = std::allocator<Key>>
struct SSet final : public std::set<Key, Compare, Allocator>, public SCollection<Key> {
  explicit SSet(const Compare &comp = Compare(),
                const Allocator &alloc = Allocator()) : std::set<Key, Compare, Allocator>(comp, alloc) {}
  explicit SSet(std::initializer_list<Key> init,
                const Compare &comp = Compare(),
                const Allocator &alloc = Allocator()) : std::set<Key, Compare, Allocator>(init, comp, alloc) {}
  SSet(const SSet &) = default;
  SSet(SSet &&) noexcept = default;

  SSet &operator=(const SSet &) = default;
  SSet &operator=(SSet &&) noexcept = default;
  ~SSet() = default;

  streams::Stream<Key, Key> stream() override {
    return streams::Stream<Key, Key>(this->begin(), this->end());
  }

  std::shared_ptr<iterators::Iterator<Key>> iterator() override {
    return std::make_shared<iterators::ListIteratorView<SSet>>(*this);
  }
};
}// namespace aalbatross::utils::collection
#endif//INCLUDED_STREAMS4CPP_STREAMEDSET_H
