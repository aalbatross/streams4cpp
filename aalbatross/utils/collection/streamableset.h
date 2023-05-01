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
                const Allocator &alloc = Allocator()) : std::set<Key, Compare, Allocator>(comp, alloc),
                                                        dIterator_(std::make_unique<iterators::ListIterator<typename std::set<Key>::iterator>>(this->begin(), this->end())) {}
  explicit SSet(std::initializer_list<Key> init,
                const Compare &comp = Compare(),
                const Allocator &alloc = Allocator()) : std::set<Key, Compare, Allocator>(init, comp, alloc),
                                                        dIterator_(std::make_unique<iterators::ListIterator<typename std::set<Key>::iterator>>(this->begin(), this->end())) {}

  SSet(const SSet &) = default;
  SSet(SSet &&) noexcept = default;

  SSet &operator=(const SSet &) = default;
  SSet &operator=(SSet &&) noexcept = default;
  ~SSet() = default;

  streams::Stream<Key, Key> stream() override {
    dIterator_ = std::move(std::make_unique<iterators::ListIterator<typename std::set<Key>::iterator>>(this->begin(), this->end()));
    return streams::Stream<Key, Key>(*dIterator_);
  }

  iterators::Iterator<Key> &iterator() override {
    dIterator_ = std::move(std::make_unique<iterators::ListIterator<typename std::set<Key>::iterator>>(this->begin(), this->end()));
    return *dIterator_;
  }

 private:
  std::unique_ptr<iterators::Iterator<Key>> dIterator_;
};
}// namespace aalbatross::utils::collection
#endif//INCLUDED_STREAMS4CPP_STREAMEDSET_H
