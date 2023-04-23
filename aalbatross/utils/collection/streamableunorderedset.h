#ifndef INCLUDED_STREAMS4CPP_STREAMEDUNORDEREDSET_H
#define INCLUDED_STREAMS4CPP_STREAMEDUNORDEREDSET_H
#include "aalbatross/utils/iterators/listiterator.h"
#include "streamablecollection.h"

#include <unordered_set>

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
  explicit SUSet(const Allocator &alloc = Allocator()) : std::unordered_set<Key, Hash, KeyEqual, Allocator>(alloc),
                                                         dIterator_(std::make_unique<iterators::ListIterator<typename std::unordered_set<Key>::iterator>>(this->begin(), this->end())) {}
  SUSet(std::initializer_list<Key> init,
        size_t bucket_count = 10,
        const Hash &hash = Hash(),
        const KeyEqual &equal = KeyEqual(),
        const Allocator &alloc = Allocator()) : std::unordered_set<Key, Hash, KeyEqual, Allocator>(init, bucket_count, hash, equal, alloc),
                                                dIterator_(std::make_unique<iterators::ListIterator<typename std::unordered_set<Key>::iterator>>(this->begin(), this->end())) {}

  ~SUSet() = default;

  streams::Stream<Key, Key> stream() override {
    dIterator_ = std::move(std::make_unique<iterators::ListIterator<typename std::unordered_set<Key>::iterator>>(this->begin(), this->end()));
    return streams::Stream<Key, Key>(*dIterator_);
  }

  iterators::Iterator<Key> &iterator() override {
    dIterator_ = std::move(std::make_unique<iterators::ListIterator<typename std::unordered_set<Key>::iterator>>(this->begin(), this->end()));
    return *dIterator_;
  }

 private:
  std::unique_ptr<iterators::Iterator<Key>> dIterator_;
};
}// namespace aalbatross::utils::collection
#endif//INCLUDED_STREAMS4CPP_STREAMEDUNORDEREDSET_H
