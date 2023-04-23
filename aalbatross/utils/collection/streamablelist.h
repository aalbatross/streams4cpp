#ifndef INCLUDED_STREAMS4CPP_STREAMEDLIST_H
#define INCLUDED_STREAMS4CPP_STREAMEDLIST_H

#include "aalbatross/utils/iterators/listiterator.h"
#include "streamablecollection.h"

#include <list>
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

  SList() : std::list<S, Allocator>(), dIterator_(std::make_unique<iterators::ListIterator<typename std::list<S>::iterator>>(this->begin(), this->end())) {}

  SList(std::initializer_list<S> init,
        const Allocator &alloc = Allocator()) : std::list<S, Allocator>(init, alloc), dIterator_(std::make_unique<iterators::ListIterator<typename std::list<S>::iterator>>(this->begin(), this->end())) {}

  ~SList() = default;

  streams::Stream<S, S> stream() override {
    dIterator_ = std::move(std::make_unique<iterators::ListIterator<typename std::list<S>::iterator>>(this->begin(), this->end()));
    return streams::Stream<S, S>(*dIterator_);
  }

  iterators::Iterator<S> &iterator() override {
    dIterator_ = std::move(std::make_unique<iterators::ListIterator<typename std::list<S>::iterator>>(this->begin(), this->end()));
    return *dIterator_;
  }

 private:
  std::unique_ptr<iterators::Iterator<S>> dIterator_;
};

template<typename S, typename Allocator = std::allocator<S>>
SList(std::initializer_list<S> init, const Allocator &alloc = Allocator()) -> SList<S, Allocator>;
}// namespace aalbatross::utils::collection
#endif//INCLUDED_STREAMS4CPP_STREAMEDLIST_H
