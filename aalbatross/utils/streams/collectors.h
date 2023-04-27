#ifndef INCLUDED_STREAMS4CPP_COLLECTORS_H_
#define INCLUDED_STREAMS4CPP_COLLECTORS_H_
#include "collector.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <type_traits>
#include <vector>
namespace aalbatross::utils::streams {
/**
 * \class Collectors
 * \brief Implementations of Collector that implement various useful reduction operations, such as accumulating elements into collections, summarizing elements according to various criteria, etc.
*
*    The following are examples of using the predefined collectors to perform common mutable reduction tasks:
*
*
*    // Accumulate names into a List <br/>
*    auto list = people.stream()
*                   .map([](auto people){return people.name;})
*                   .collect(Collectors::toVector<std::string>());
*
*    // Accumulate names into a TreeSet <br/>
*    auto set = people.stream()
*                      .map([](auto people){return people.name;})
*                      .collect(Collectors.toSet<std::string>());
*
*    // Convert elements to strings and concatenate them, separated by commas <br/>
*    auto joined = things.stream()
*                    .map([](auto thing){return toString(thing);})
*                    .collect(Collectors::joining(", "));
*
*    // Compute sum of salaries of employee <br/>
*    auto total = employees.stream()
*                .collect(Collectors::summingDouble([](auto emp){return emp.salary;}));
*
*    // Group employees by department <br/>
*    std::map<Department, std::vector<Employee>> byDept = employees.stream()
*                                             .collect(Collectors.groupingBy<Employee>([](auto emp){return emp.dept;}));
*
*    // Compute sum of salaries by department <br/>
*    std::map<Department, double> totalByDept = employees.stream()
*                                           .collect(Collectors.groupingBy<Employee>([](auto emp){return emp.dept;},
*                                                                          Collectors::summingDouble([](auto emp){return emp.salary;})));
*
*    // Partition students into passing and failing <br/>
*    std::map<bool, std::vector<Student>> passingFailing = students.stream()
*                                                 .collect(Collectors::partitioningBy<Student>([](auto student) {return s.getGrade() >= PASS_THRESHOLD}));
*
*
*/
struct Collectors final {
  /**
  * \fn auto averaging(TypeToDouble &&mapper)
  * \brief Returns a Collector that produces the arithmetic mean of a double-valued function applied to the input elements. If no elements are present, the result is 0.
  * @tparam TypeToDouble a function extracting the property to be averaged
  * @param mapper
  * @return a Collector that produces the arithmetic mean of a derived property
  */
  template<typename TypeToDouble>
  static auto averaging(TypeToDouble &&mapper) {
    return streams::Collector{[] { return std::vector<double>(); },
                              [mapper](std::vector<double> &intermediate, auto element) {
                                intermediate.emplace_back(mapper(element));
                              },
                              [](std::vector<double> &intermediate) -> double {
                                double sum = 0;
                                for (const double &item : intermediate) {
                                  sum += item;
                                }
                                return (sum / intermediate.size());
                              }};
  }

  /**
   * \fn auto counting()
   * \brief Returns a Collector accepting elements of type T that counts the number of input elements.
   * @return a Collector that counts the input elements
   */
  static auto counting() {
    return streams::Collector{[] { return std::vector<size_t>(); },
                              [](std::vector<size_t> &intermediate, auto element) {
                                intermediate.emplace_back(1);
                              },
                              [](std::vector<size_t> &intermediate) -> size_t {
                                size_t sum = 0;
                                for (const auto &item : intermediate) {
                                  sum += item;
                                }
                                return sum;
                              }};
  }

  /**
   * \fn auto summingLong(TypeToLong &&mapper)
   * \brief Returns a Collector that produces the sum of a long-valued function applied to the input elements.
   * @tparam TypeToLong Type of function extracting the property to be summed
   * @param mapper
   * @return a Collector that produces the sum of a derived property
   */
  template<typename TypeToLong>
  static auto summingLong(TypeToLong &&mapper) {
    return streams::Collector{[] { return std::vector<long>(); },
                              [mapper](std::vector<long> &intermediate, auto element) {
                                intermediate.emplace_back(mapper(element));
                              },
                              [](std::vector<long> &intermediate) -> long {
                                long sum = 0;
                                for (const long &item : intermediate) {
                                  sum += item;
                                }
                                return sum;
                              }};
  }

  /**
   * \fn auto summingDouble(TypeToDouble &&mapper)
   * \brief Returns a Collector that produces the sum of a double-valued function applied to the input elements.
   * @tparam TypeToDouble Type of function extracting the property to be summed
   * @param mapper
   * @return a Collector that produces the sum of a derived property
   */
  template<typename TypeToDouble>
  static auto summingDouble(TypeToDouble &&mapper) {
    return streams::Collector{[] { return std::vector<double>(); },
                              [mapper](std::vector<double> &intermediate, auto element) {
                                intermediate.emplace_back(mapper(element));
                              },
                              [](std::vector<double> &intermediate) -> double {
                                double sum = 0;
                                for (const double &item : intermediate) {
                                  sum += item;
                                }
                                return sum;
                              }};
  }

  /**
   * \fn auto collectingAndThen(Collector<Supplier, Accumulator, Finisher> &&collector, Mapper &&mapper)
   * \brief Adapts a Collector to perform an additional finishing transformation.
   * @tparam Supplier Type of Supplier Function of the collector
   * @tparam Accumulator Type of Accumulator Function of the collector
   * @tparam Finisher Type of the Finisher Function of the collector
   * @tparam Mapper type of Finisher function to be applied to the final result of the downstream collector
   * @param collector
   * @param mapper
   * @return a collector which performs the action of the downstream collector, followed by an additional finishing step
   */
  template<typename Supplier, typename Accumulator, typename Finisher, typename Mapper>
  static auto collectingAndThen(Collector<Supplier, Accumulator, Finisher> &&collector, Mapper &&mapper) {
    return streams::Collector{collector.supplier(),
                              collector.accumulator(),
                              [collector, mapper](decltype(collector.supplier()()) &intermediate) {
                                return mapper(collector.finisher()(intermediate));
                              }};
  }

  /**
   * \fn auto groupingBy(Classifier &&mapper, Compare cmp = Compare(), Allocator allocator = Allocator())
   * \brief Returns a Collector implementing a "group by" operation on input elements of type T, grouping elements according to a classification function, and returning the results in a Map.
   * @tparam T input element type
   * @tparam Classifier the type of classifier function mapping input elements to keys
   * @tparam K Key type
   * @tparam Compare Comparator for resulting map
   * @tparam Allocator Allocator for resulting map
   * @param mapper
   * @param cmp
   * @param allocator
   * @return a Collector implementing the group-by operation
   */
  template<typename T, typename Classifier, typename K = typename std::invoke_result_t<Classifier, T>, typename Compare = std::less<K>,
           class Allocator = std::allocator<std::pair<const K, std::vector<T>>>>
  static auto groupingBy(Classifier &&mapper, Compare cmp = Compare(), Allocator allocator = Allocator()) {
    return streams::Collector{[] { return std::vector<std::pair<K, T>>(); },
                              [mapper](std::vector<std::pair<K, T>> &intermediate, T element) {
                                intermediate.emplace_back(std::pair<K, T>{mapper(element), element});
                              },
                              [cmp, allocator](auto &intermediate) {
                                std::map<K, std::vector<T>, Compare, Allocator> result(cmp, allocator);
                                for (auto item : intermediate) {
                                  result.try_emplace(item.first, std::vector<T>());
                                  result[item.first].emplace_back(item.second);
                                }
                                return result;
                              }};
  }

  /**
   * \fn auto groupingBy(Classifier &&mapper, Collector<Supplier, Accumulator, Finisher> &&collector, Compare cmp = Compare(), Allocator allocator = Allocator())
   * \brief Returns a Collector implementing a cascaded "group by" operation on input elements of type T, grouping elements according to a classification function, and then performing a reduction operation on the values associated with a given key using the specified downstream Collector.
   *
   * There are no guarantees on the type, mutability, serializability, or thread-safety of the Map returned.
   * // Compute sum of salaries by department <br/>
   *    std::map<Department, double> totalByDept = employees.stream()
   *                                           .collect(Collectors.groupingBy<Employee>([](auto emp){return emp.dept;},
   *                                                                          Collectors::summingDouble([](auto emp){return emp.salary;})));
   * @tparam T type of input element
   * @tparam Classifier type of classifier function mapping input elements to keys
   * @tparam K type of keys
   * @tparam Supplier type of Supplier of downstream collector
   * @tparam Accumulator type of Accumulator of downstream collector
   * @tparam Finisher type of Finisher of downstream collector
   * @tparam Compare type of Comparator for returning map
   * @tparam Allocator type of Allocator for returning map
   * @param mapper
   * @param collector
   * @param cmp
   * @param allocator
   * @return a Collector implementing the cascaded group-by operation
   */
  template<typename T, typename Classifier, typename K = typename std::invoke_result_t<Classifier, T>, typename Supplier, typename Accumulator, typename Finisher, typename Compare = std::less<K>,
           class Allocator = std::allocator<std::pair<const K, std::vector<T>>>>
  static auto groupingBy(Classifier &&mapper, Collector<Supplier, Accumulator, Finisher> &&collector, Compare cmp = Compare(), Allocator allocator = Allocator()) {
    return streams::Collector{[] { return std::vector<std::pair<K, T>>(); },
                              [mapper](std::vector<std::pair<K, T>> &intermediate, T element) {
                                intermediate.emplace_back(std::pair<K, T>{mapper(element), element});
                              },
                              [&collector, &cmp, &allocator](auto &intermediate) {
                                std::map<K, std::vector<T>, Compare, Allocator> iResult(cmp, allocator);
                                for (auto item : intermediate) {
                                  iResult.try_emplace(item.first, std::vector<T>());
                                  iResult[item.first].emplace_back(item.second);
                                }
                                auto repr = (*iResult.begin()).second;
                                using X = decltype(collector.apply(repr));
                                std::map<K, X, Compare> result(cmp);
                                for (auto item : iResult) {
                                  result.insert({item.first, collector.apply(item.second)});
                                }
                                return result;
                              }};
  }

  /**
   * \fn auto joining(std::string delimiter = " ", std::string prefix = "", std::string suffix = "")
   * @param delimiter
   * @param prefix
   * @param suffix
   * @return
   */
  static auto joining(std::string delimiter = " ", std::string prefix = "", std::string suffix = "") {
    return streams::Collector{[] { return std::vector<std::string>(); },
                              [](std::vector<std::string> &intermediate, auto element) {
                                intermediate.emplace_back(element);
                              },
                              [&](std::vector<std::string> &intermediate) -> std::string {
                                std::stringstream sstream;
                                sstream << prefix;
                                for (size_t i = 0; i < intermediate.size() - 1; i++) {
                                  sstream << intermediate[i] << delimiter;
                                }
                                sstream << intermediate[intermediate.size() - 1] << suffix;
                                return sstream.str();
                              }};
  }

  template<typename T, typename Comparator>
  static auto maxBy(Comparator &&comp) {
    return streams::Collector{[] { return std::vector<T>(); },
                              [](std::vector<T> &intermediate, auto element) {
                                intermediate.emplace_back(element);
                              },
                              [&](std::vector<T> &intermediate) -> std::optional<T> {
                                auto iterator = std::max_element(intermediate.begin(), intermediate.end(), comp);
                                return iterator != intermediate.end() ? std::optional<T>(*iterator) : std::nullopt;
                              }};
  }

  template<typename T, typename Comparator>
  static auto minBy(Comparator &&comp) {
    return streams::Collector{[] { return std::vector<T>(); },
                              [](std::vector<T> &intermediate, auto element) {
                                intermediate.emplace_back(element);
                              },
                              [&](std::vector<T> &intermediate) -> std::optional<T> {
                                auto iterator = std::min_element(intermediate.begin(), intermediate.end(), comp);
                                return iterator != intermediate.end() ? std::optional<T>(*iterator) : std::nullopt;
                              }};
  }

  template<typename T, typename Predicate>
  static auto partitioningBy(Predicate &&predicate) {
    return streams::Collector{[] { return std::vector<T>(); },
                              [](std::vector<T> &intermediate, auto element) {
                                intermediate.emplace_back(element);
                              },
                              [predicate](std::vector<T> &intermediate) {
                                std::map<bool, std::vector<T>> result;
                                for (auto &item : intermediate) {
                                  result.try_emplace(predicate(item), std::vector<T>());
                                  result[predicate(item)].emplace_back(item);
                                }
                                return result;
                              }};
  }

  template<typename T, typename Predicate, typename Supplier, typename Accumulator, typename Finisher>
  static auto partitioningBy(Predicate &&predicate, Collector<Supplier, Accumulator, Finisher> &&downstream) {
    return streams::Collector{[] { return std::vector<T>(); },
                              [](std::vector<T> &intermediate, auto element) {
                                intermediate.emplace_back(element);
                              },
                              [predicate, downstream](std::vector<T> &intermediate) {
                                std::map<bool, std::vector<T>> iResult;
                                for (auto &item : intermediate) {
                                  iResult.try_emplace(predicate(item), std::vector<T>());
                                  iResult[predicate(item)].emplace_back(item);
                                }
                                auto repr = (*iResult.begin()).second;
                                using X = decltype(downstream.apply(repr));
                                std::map<bool, X> result;
                                for (std::pair<const bool, std::vector<T>> &item : iResult) {
                                  result.insert(std::pair<bool, X>{item.first, downstream.apply(item.second)});
                                }
                                return result;
                              }};
  }

  template<typename Mapper, typename Supplier, typename Accumulator, typename Finisher>
  static auto mapping(Mapper &&mapper, Collector<Supplier, Accumulator, Finisher> &&downstream) {
    return streams::Collector{downstream.supplier(),
                              [mapper, downstream](decltype(downstream.supplier()()) &intermediate, auto element) {
                                downstream.accumulator()(intermediate, mapper(element));
                              },
                              downstream.finisher()};
  }

  template<typename T>
  static auto toVector() {
    return streams::Collector{[] { return std::vector<T>(); },
                              [](std::vector<T> &intermediate, auto element) {
                                intermediate.emplace_back(element);
                              },
                              [](std::vector<T> &intermediate) {
                                return intermediate;
                              }};
  }

  template<typename T, typename Compare = std::less<T>,
           typename Allocator = std::allocator<T>>
  static auto toSet(Compare cmp = Compare(), Allocator allocator = Allocator()) {
    return streams::Collector{[cmp, allocator] { return std::set<T, Compare, Allocator>(cmp, allocator); },
                              [](std::set<T, Compare, Allocator> &intermediate, auto element) {
                                intermediate.emplace(element);
                              },
                              [](std::set<T, Compare, Allocator> &intermediate) {
                                return intermediate;
                              }};
  }

  template<typename T, typename KeyMapper, typename ValueMapper>
  static auto toMap(KeyMapper &&keyMapper, ValueMapper &&valueMapper) {
    return streams::Collector{[] { return std::vector<T>(); },
                              [](std::vector<T> &intermediate, auto element) {
                                intermediate.emplace_back(element);
                              },
                              [keyMapper, valueMapper](std::vector<T> &intermediate) {
                                using K = typename std::invoke_result<KeyMapper, T>::type;
                                using V = typename std::invoke_result<ValueMapper, T>::type;

                                std::map<K, V, std::less<K>, std::allocator<std::pair<const K, V>>> result;
                                for (const T &item : intermediate) {
                                  result[keyMapper(item)] = valueMapper(item);
                                }
                                return result;
                              }};
  }

  template<typename T, typename KeyMapper, typename ValueMapper, typename MergeFunction>
  static auto toMap(KeyMapper &&keyMapper, ValueMapper &&valueMapper, MergeFunction &&mergeFunction) {
    return streams::Collector{[] { return std::vector<T>(); },
                              [](std::vector<T> &intermediate, auto element) {
                                intermediate.emplace_back(element);
                              },
                              [keyMapper, valueMapper, mergeFunction](std::vector<T> &intermediate) {
                                using K = typename std::invoke_result<KeyMapper, T>::type;
                                using V = typename std::invoke_result<ValueMapper, T>::type;

                                std::map<K, std::vector<V>> iResult;
                                for (const T &item : intermediate) {
                                  iResult.try_emplace(keyMapper(item), std::vector<V>());
                                  iResult[keyMapper(item)].emplace_back(valueMapper(item));
                                }

                                std::map<K, V> result;
                                for (const auto &item : iResult) {
                                  result[item.first] = std::reduce(item.second.begin(), item.second.end(), V{}, mergeFunction);
                                }

                                return result;
                              }};
  }

  template<typename T, typename BinaryOp>
  static auto reducing(BinaryOp &&binaryOp) {
    return streams::Collector{
        [] { return std::vector<T>(); },
        [](std::vector<T> &intermediate, auto element) {
          intermediate.emplace_back(element);
        },
        [&binaryOp](std::vector<T> &intermediate) {
          return std::reduce(intermediate.begin(), intermediate.end(), T{}, binaryOp);
        }};
  }

  template<typename T, typename BinaryOp>
  static auto reducing(T identity, BinaryOp &&binaryOp) {
    return streams::Collector{
        [] { return std::vector<T>(); },
        [](std::vector<T> &intermediate, auto element) {
          intermediate.emplace_back(element);
        },
        [&identity, &binaryOp](std::vector<T> &intermediate) {
          return std::reduce(intermediate.begin(), intermediate.end(), identity, binaryOp);
        }};
  }
};
}// namespace aalbatross::utils::streams
#endif
