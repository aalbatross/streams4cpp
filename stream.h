#ifndef STREAMS4CPP__STREAM_H_
#define STREAMS4CPP__STREAM_H_
#include "iterator.h"
#include "listiterator.h"
namespace aalbatross::util {

    template<typename T, typename S = T>
    struct Stream {
        Stream(Iterator<S> &source) : d_source(source) {
            std::function<std::unique_ptr<Iterator<T>>(Iterator<S> &)> x =
                    [](Iterator<S> &source) {
                        std::vector<S> x;
                        while (source.hasNext()) {
                            x.emplace_back(source.next());
                        }
                        return std::make_unique(x.begin(), x.end());
                    };
            d_mapper = x;
        }

        template<typename E>
        Stream<E, S> map(std::function<E(T)> mapper) {
            std::function<std::unique_ptr<Iterator<E>>(Iterator<S> &)> newMapper =
                    [&](const auto source) {
                        auto inter = d_mapper(source);
                        std::vector<E> result;
                        while (inter->hasNext()) {
                            result.emplace_back(inter->next());
                        }
                        return std::make_unique<ListIterator<std::vector<E>, E>>(
                                result.begin(), result.end());
                    };
            return Stream<E, S>(d_source, newMapper);
        }

        Stream<T, S> filter(std::function<bool(T)> predicate) {
            std::function<std::unique_ptr<Iterator<T>>(Iterator<S> &)> newMapper =
                    [&](const auto source) {
                        auto inter = d_mapper(source);
                        std::vector<T> result;
                        while (inter->hasNext()) {
                            const auto x = inter->next();
                            if (predicate(x)) {
                                result.emplace_back(x);
                            }
                        }
                        return std::make_unique(ListIterator(result.begin(), result.end()));
                    };
            return Stream<T, S>(d_source, newMapper);
        }

        Stream<T, S> skip(int count) {
            std::function<Iterator<T>(Iterator<S> &)> newMapper =
                    [&](const auto source) {
                        auto inter = d_mapper(source);
                        std::vector<T> result;
                        result.reserve(count);
                        int total = 0;
                        while (inter->hasNext()) {
                            const auto x = inter->next();
                            if (total < count) {
                                result.emplace_back(x);
                                total++;
                            }
                        }
                        return std::make_unique(ListIterator(result.begin(), result.end()));
                    };
            return Stream<T, S>(d_source, newMapper);
        }

        T reduce(T identity, std::function<T(T, T)> binaryAccumulator) {
            auto result = d_mapper(d_source);
            T output = identity;
            while (result->hasNext()) {
                output = binaryAccumulator(output, result->next());
            }
            return output;
        }

    private:
        Stream(Iterator<S> &source,
               std::function<std::unique_ptr<Iterator<T>>(Iterator<S> &)> mapper)
            : d_source(source), d_mapper(mapper) {}

        std::function<std::unique_ptr<Iterator<T>>(Iterator<S> &)> d_mapper;
        Iterator<S> &d_source;
    };
};// namespace aalbatross::util

#endif// STREAMS4CPP__STREAM_H_
