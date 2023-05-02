#ifndef INCLUDED_STREAMS4CPP_STREAMEDMAP_H
#define INCLUDED_STREAMS4CPP_STREAMEDMAP_H

#include "aalbatross/utils/iterators/listiterator.h"
#include "streamablecollection.h"

#include <functional>
#include <map>
#include <memory>
#include <utility>

namespace aalbatross::utils::collection {
/**
 * \class SMap
 * \brief Streamable Map class extends STL Map and implements StreamableCollection interface.
 *
 * Streamable Map can be used to transform underlying Map using map/reduce style operations on Stream.
 * @tparam Key
 * @tparam T
 * @tparam Compare
 * @tparam Allocator
 */
template<
    typename Key,
    typename T,
    typename Compare = std::less<Key>,
    typename Allocator = std::allocator<std::pair<const Key, T>>>
struct SMap final : public std::map<Key, T, Compare, Allocator>, public SCollection<std::pair<const Key, T>> {

  explicit SMap(const Compare &comp = Compare(),
                const Allocator &alloc = Allocator()) : std::map<Key, T, Compare, Allocator>(comp, alloc) {}
  explicit SMap(std::initializer_list<std::pair<const Key, T>> init,
                const Compare &comp = Compare(),
                const Allocator &alloc = Allocator()) : std::map<Key, T>(init, comp, alloc) {}
  SMap(const SMap &) = default;
  SMap(SMap &&) noexcept = default;

  SMap &operator=(const SMap &) = default;
  SMap &operator=(SMap &&) noexcept = default;
  ~SMap() = default;

  streams::Stream<std::pair<const Key, T>, std::pair<const Key, T>> stream() override {
    return streams::Stream<std::pair<const Key, T>, std::pair<const Key, T>>(this->begin(), this->end());
  }

  std::shared_ptr<iterators::Iterator<std::pair<const Key, T>>> iterator() override {
    return std::make_shared<iterators::ListIteratorView<SMap>>(*this);
  }
};

template<
    typename Key,
    typename T,
    typename Compare = std::less<Key>,
    typename Allocator = std::allocator<std::pair<const Key, T>>>
SMap(std::initializer_list<std::pair<Key, T>>,
     const Compare &comp,
     const Allocator &alloc)
    -> SMap<Key, T, Compare, Allocator>;

}// namespace aalbatross::utils::collection

#endif//INCLUDED_STREAMS4CPP_STREAMEDMAP_H
