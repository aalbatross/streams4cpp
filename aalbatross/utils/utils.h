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
