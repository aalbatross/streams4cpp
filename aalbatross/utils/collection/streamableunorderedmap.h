#ifndef INCLUDED_STREAMS4CPP_STREAMEDUNORDEREDMAP_H
#define INCLUDED_STREAMS4CPP_STREAMEDUNORDEREDMAP_H

#include "aalbatross/utils/iterators/listiterator.h"
#include "streamablecollection.h"

#include <memory>
#include <unordered_map>
#include <utility>
namespace aalbatross::utils::collection {
/**
 * \class SUMap
 * \brief Streamable Unordered Map class extends STL Unordered Map and implements StreamableCollection interface.
 *
 * Streamable UnorderedMap can be used to transform Map using map/reduce style operations on Stream.
 * @tparam Key key type
 * @tparam T value type
 * @tparam Hash type of class calculating hash
 * @tparam KeyEqual type of class calculating equal_to on key
 * @tparam Allocator
 */
template<
    typename Key,
    typename T,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>,
    typename Allocator = std::allocator<std::pair<const Key, T>>>
struct SUMap final : public std::unordered_map<Key, T, Hash, KeyEqual, Allocator>, public SCollection<std::pair<const Key, T>> {

  explicit SUMap(const Allocator &alloc = Allocator()) : std::unordered_map<Key, T, Hash, KeyEqual, Allocator>(alloc) {}
  explicit SUMap(std::initializer_list<std::pair<const Key, T>> init,
                 size_t bucket_count = NUM_OF_BUCKETS,
                 const Hash &hash = Hash(),
                 const KeyEqual &equal = KeyEqual(),
                 const Allocator &alloc = Allocator()) : std::unordered_map<Key, T, Hash, KeyEqual, Allocator>(init, bucket_count, hash, equal, alloc) {}
  SUMap(const SUMap &) = default;
  SUMap(SUMap &&) noexcept = default;

  SUMap &operator=(const SUMap &) = default;
  SUMap &operator=(SUMap &&) noexcept = default;
  ~SUMap() = default;

  streams::Stream<std::pair<const Key, T>, std::pair<const Key, T>> stream() override {
    return streams::Stream<std::pair<const Key, T>, std::pair<const Key, T>>(this->begin(), this->end());
  }

  std::shared_ptr<iterators::Iterator<std::pair<const Key, T>>> iterator() override {
    return std::make_shared<iterators::ListIteratorView<SUMap>>(*this);
  }

 private:
  static const size_t NUM_OF_BUCKETS = 10;
};

template<
    typename Key,
    typename T,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>,
    typename Allocator = std::allocator<std::pair<const Key, T>>>
SUMap(std::initializer_list<std::pair<Key, T>>,
      size_t bucket_count,
      const Hash &hash,
      const KeyEqual &equal,
      const Allocator &alloc)
    -> SUMap<Key, T, Hash, KeyEqual, Allocator>;

}// namespace aalbatross::utils::collection

#endif//INCLUDED_STREAMS4CPP_STREAMEDUNORDEREDMAP_H
