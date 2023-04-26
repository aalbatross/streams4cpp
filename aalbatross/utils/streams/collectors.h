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

struct Collectors {

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

  static auto summingLong() {
    return streams::Collector{[] { return std::vector<long>(); },
                              [](std::vector<long> &intermediate, auto element) {
                                intermediate.emplace_back(element);
                              },
                              [](std::vector<long> &intermediate) -> long {
                                long sum = 0;
                                for (const long &item : intermediate) {
                                  sum += item;
                                }
                                return sum;
                              }};
  }

  static auto summingDouble() {
    return streams::Collector{[] { return std::vector<double>(); },
                              [](std::vector<double> &intermediate, auto element) {
                                intermediate.emplace_back(element);
                              },
                              [](std::vector<double> &intermediate) -> double {
                                double sum = 0;
                                for (const double &item : intermediate) {
                                  sum += item;
                                }
                                return sum;
                              }};
  }

  template<typename Supplier, typename Accumulator, typename Finisher, typename Mapper>
  static auto collectingAndThen(Collector<Supplier, Accumulator, Finisher> &&collector, Mapper &&mapper) {
    return streams::Collector{collector.supplier(),
                              collector.accumulator(),
                              [collector, mapper](decltype(collector.supplier()()) &intermediate) {
                                return mapper(collector.finisher()(intermediate));
                              }};
  }

  template<typename K, typename T, typename Classifier>
  static auto groupingBy(Classifier &&mapper) {
    return streams::Collector{[] { return std::vector<std::pair<K, T>>(); },
                              [mapper](std::vector<std::pair<K, T>> &intermediate, T element) {
                                intermediate.emplace_back(std::pair<K, T>{mapper(element), element});
                              },
                              [](auto &intermediate) {
                                std::unordered_map<K, std::vector<T>> result;
                                for (auto item : intermediate) {
                                  result.try_emplace(item.first, std::vector<T>());
                                  result[item.first].emplace_back(item.second);
                                }
                                return result;
                              }};
  }

  template<typename K, typename T, typename Classifier, typename Supplier, typename Accumulator, typename Finisher>
  static auto groupingBy(Classifier &&mapper, Collector<Supplier, Accumulator, Finisher> &&collector) {
    return streams::Collector{[] { return std::vector<std::pair<K, T>>(); },
                              [mapper](std::vector<std::pair<K, T>> &intermediate, T element) {
                                intermediate.emplace_back(std::pair<K, T>{mapper(element), element});
                              },
                              [&](auto &intermediate) {
                                std::unordered_map<K, std::vector<T>> iResult;
                                for (auto item : intermediate) {
                                  iResult.try_emplace(item.first, std::vector<T>());
                                  iResult[item.first].emplace_back(item.second);
                                }
                                auto repr = (*iResult.begin()).second;
                                using X = decltype(collector.apply(repr));
                                std::unordered_map<K, X> result;
                                for (auto item : iResult) {
                                  result.insert({item.first, collector.apply(item.second)});
                                }
                                return result;
                              }};
  }

  static auto joining(std::string delimiter = " ", std::string prefix = "", std::string suffix = "") {
    return streams::Collector{[] { return std::vector<std::string>(); },
                              [](std::vector<std::string> &intermediate, auto element) {
                                intermediate.emplace_back(element);
                              },
                              [&](std::vector<std::string> &intermediate) -> std::string {
                                std::stringstream ss;
                                ss << prefix;
                                for (size_t i = 0; i < intermediate.size() - 1; i++) {
                                  ss << intermediate[i] << delimiter;
                                }
                                ss << intermediate[intermediate.size() - 1] << suffix;
                                return ss.str();
                              }};
  }

  template<typename T, typename Comparator>
  static auto maxBy(Comparator &&comp) {
    return streams::Collector{[] { return std::vector<T>(); },
                              [](std::vector<T> &intermediate, auto element) {
                                intermediate.emplace_back(element);
                              },
                              [&](std::vector<T> &intermediate) -> std::optional<T> {
                                auto it = std::max_element(intermediate.begin(), intermediate.end(), comp);
                                return it != intermediate.end() ? std::optional<T>(*it) : std::nullopt;
                              }};
  }

  template<typename T, typename Comparator>
  static auto minBy(Comparator &&comp) {
    return streams::Collector{[] { return std::vector<T>(); },
                              [](std::vector<T> &intermediate, auto element) {
                                intermediate.emplace_back(element);
                              },
                              [&](std::vector<T> &intermediate) -> std::optional<T> {
                                auto it = std::min_element(intermediate.begin(), intermediate.end(), comp);
                                return it != intermediate.end() ? std::optional<T>(*it) : std::nullopt;
                              }};
  }

  template<typename T, typename Predicate>
  static auto partitioningBy(Predicate &&predicate) {
    return streams::Collector{[] { return std::vector<T>(); },
                              [](std::vector<T> &intermediate, auto element) {
                                intermediate.emplace_back(element);
                              },
                              [predicate](std::vector<T> &intermediate) {
                                std::unordered_map<bool, std::vector<T>> result;
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
                                std::unordered_map<bool, std::vector<T>> iResult;
                                for (auto &item : intermediate) {
                                  iResult.try_emplace(predicate(item), std::vector<T>());
                                  iResult[predicate(item)].emplace_back(item);
                                }
                                auto repr = (*iResult.begin()).second;
                                using X = decltype(downstream.apply(repr));
                                std::unordered_map<bool, X> result;
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

  template<typename T>
  static auto toSet() {
    return streams::Collector{[] { return std::set<T>(); },
                              [](std::set<T> &intermediate, auto element) {
                                intermediate.emplace(element);
                              },
                              [](std::set<T> &intermediate) {
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

                                std::map<K, V> result;
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
