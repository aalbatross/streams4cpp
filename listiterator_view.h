#ifndef STREAMS4CPP__LISTITERATOR_VIEW_H_
#define STREAMS4CPP__LISTITERATOR_VIEW_H_
#include "iterator.h"
namespace aalbatross::util {

    template<typename Container,
             typename T = typename Container::value_type>
    struct ListIteratorView : public Iterator<T> {
        ListIteratorView(const Container &storage)
            : d_data(storage), d_begin(storage.begin()), d_end(storage.end()), d_current(storage.begin()), d_last(*storage.begin()) {}

        ~ListIteratorView() = default;

        inline bool hasNext() override { return d_current != d_end; }

        inline T next() override {
            d_last = *d_current;
            std::advance(d_current, 1);
            return d_last;
        }

        inline void reset() override { d_current = d_begin; }

    private:
        const Container d_data;
        decltype(d_data.begin()) d_begin;
        decltype(d_data.end()) d_end;
        decltype(d_data.begin()) d_current;
        T d_last;
    };

    template<typename Container,
             typename T = typename Container::value_type>
    ListIteratorView(Container &) -> ListIteratorView<Container, T>;
};// namespace aalbatross::util


#endif//STREAMS4CPP__LISTITERATOR_VIEW_H_
