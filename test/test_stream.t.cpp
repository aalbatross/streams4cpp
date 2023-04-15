#include <gtest/gtest.h>
#include <listiterator.h>
namespace aalbatross::util::test {

    TEST(StreamTestFixture, ReturnTransformedStream) {
        std::vector x{1, 2, 3, 4, 5};
        ListIterator iter(x.begin(), x.end());
    }
};// namespace aalbatross::util::test