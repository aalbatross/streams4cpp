#ifndef INCLUDED_STREAMS4CPP_UTILS_H_
#define INCLUDED_STREAMS4CPP_UTILS_H_

namespace aalbatross {
/**
 * @brief Utility classes.
 *
 * \ingroup aalbatross::utils
 */
namespace utils {

/**
 * @brief This namespace defines Interfaces and Classes needed to implement Iterators.
 * Iterators as structure let us read objects in queryable way from any bound or unbound sources.
 * Bound sources like STL classes, and unbound sources like IO based sources stream of characters on screen
 * and stream of bytes from networks or from files.
 *
 * \ingroup aalbatross::utils::iterators
 */
namespace iterators {
struct Iterator;
struct ListIterator;
struct ListIteratorView;
}// namespace iterators

/**
 * @brief Classes to support functional-style operations on streams of elements, such as map-reduce transformations on collections.
 *
 *
 *   Streams differ from collections in several ways:
 *
 *   No storage. A stream is not a data structure that stores elements; instead, it conveys elements from a source such as a data structure, an array, a generator function, or an I/O channel, through a pipeline of computational operations.
 *   Functional in nature. An operation on a stream produces a result, but does not modify its source. For example, filtering a Stream obtained from a collection produces a new Stream without the filtered elements, rather than removing elements from the source collection.
 *   Laziness-seeking. Many stream operations, such as filtering, mapping, or duplicate removal, can be implemented lazily, exposing opportunities for optimization. For example, "find the first String with three consecutive vowels" need not examine all the input strings. Stream operations are divided into intermediate (Stream-producing) operations and terminal (value- or side-effect-producing) operations. Intermediate operations are always lazy.
 *   Possibly unbounded. While collections have a finite size, streams need not. Short-circuiting operations such as limit(n) or findFirst() can allow computations on infinite streams to complete in finite time.
 *   Consumable. The elements of a stream are only visited once during the life of a stream. Like an Iterator, a new stream must be generated to revisit the same elements of the source.
 *
 *
 *
 * \ingroup aalbatross::utils::streams
 */
namespace streams {
struct Stream;
}

/**
 * @brief Container classes supporting streamable STL containers.
 * \ingroup aalbatross::utils::collection
 */
namespace collection {
struct SCollection;
struct SDeque;
struct SList;
struct SVector;
struct SMap;
struct SSet;
struct SUSet;
struct SUMap;
}// namespace collection

}// namespace utils
}// namespace aalbatross
#endif//INCLUDED_STREAMS4CPP_UTILS_H_
