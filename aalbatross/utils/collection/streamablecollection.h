#ifndef INCLUDED_STREAMS4CPP_STREAMEDCOLLECTION_H
#define INCLUDED_STREAMS4CPP_STREAMEDCOLLECTION_H

#include <aalbatross/utils/iterators/iterator.h>
#include <aalbatross/utils/streams/stream.h>

namespace aalbatross::utils::collection {

/**
 * \class SCollection
 * \brief Interface defining Streamable Collection.
 * @tparam S element type
 */
template<typename S>
struct SCollection {
  virtual ~SCollection() = default;

  /**
   * \fn iterators::Iterator<S> &iterator()
   * \brief Provides internal Iterator reference for the underlying container.
   * @return get reference of internal Iterator of underlying container.
   */
  virtual iterators::Iterator<S> &iterator() = 0;

  /**
   * \fn streams::Stream<S, S> stream()
   * \brief Create stream from the underlying container.
   * @return stream from the underlying container.
   */
  virtual streams::Stream<S, S> stream() = 0;
};
}// namespace aalbatross::utils::collection
#endif//INCLUDED_STREAMS4CPP_STREAMEDCOLLECTION_H
