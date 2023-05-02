#ifndef INCLUDED_STREAMS4CPP_STREAMEDUNORDEREDSET_H
#define INCLUDED_STREAMS4CPP_STREAMEDUNORDEREDSET_H
#include "aalbatross/utils/iterators/listiterator.h"
#include "streamablecollection.h"

#include <functional>
#include <memory>
#include <unordered_set>
#include <utility>
namespace aalbatross::utils::collection {
/**
 * \class SUSet
 * \brief Streamable Unordered Set class extends STL Unordered Set and implements StreamableCollection interface.
 *
 * Streamable UnorderedSet can be used to transform Set using map/reduce style operations on Stream.
 * @tparam Key type of element
 * @tparam Hash type of class calculating hash
 * @tparam KeyEqual type of class calculating equals_to for Key
 * @tparam Allocator
 */
template<
    typename Key,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>,
    typename Allocator = std::allocator<Key>>
struct SUSet final : public std::unordered_set<Key, Hash, KeyEqual, Allocator>, public SCollection<Key> {
  explicit SUSet(const Allocator &alloc = Allocator()) : std::unordered_set<Key, Hash, KeyEqual, Allocator>(alloc) {}
  explicit SUSet(std::initializer_list<Key> init,
                 size_t bucket_count = NUM_OF_BUCKETS,
                 const Hash &hash = Hash(),
                 const KeyEqual &equal = KeyEqual(),
                 const Allocator &alloc = Allocator()) : std::unordered_set<Key, Hash, KeyEqual, Allocator>(init, bucket_count, hash, equal, alloc) {}
  SUSet(const SUSet &) = default;
  SUSet(SUSet &&) noexcept = default;

  SUSet &operator=(const SUSet &) = default;
  SUSet &operator=(SUSet &&) noexcept = default;
  ~SUSet() = default;

  streams::Stream<Key, Key> stream() override {
    return streams::Stream<Key, Key>(this->begin(), this->end());
  }

  std::shared_ptr<iterators::Iterator<Key>> iterator() override {
    return std::make_shared<iterators::ListIteratorView<SUSet>>(*this);
  }

 private:
  static const size_t NUM_OF_BUCKETS = 10;
};
}// namespace aalbatross::utils::collection
#endif//INCLUDED_STREAMS4CPP_STREAMEDUNORDEREDSET_H
