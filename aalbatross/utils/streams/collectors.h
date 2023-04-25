#ifndef INCLUDED_STREAMS4CPP_COLLECTORS_H_
#define INCLUDED_STREAMS4CPP_COLLECTORS_H_
#include "collector.h"

#include <iostream>
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
    //using X = typename std::invoke_result<decltype(&streams::Collector<Supplier, Accumulator, Finisher>::apply), std::vector<T> &>::type;
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
};
}// namespace aalbatross::utils::streams
#endif
