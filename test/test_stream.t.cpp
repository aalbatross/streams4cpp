#include <aalbatross/utils/listiterator.h>
#include <aalbatross/utils/stream.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
namespace aalbatross::utils::test {

TEST(StreamTestFixture, ReturnTransformedStream) {
  std::vector x{1, 2, 3, 4, 5};
  ListIterator iter(x.begin(), x.end());
  Stream<int> s(iter);
  auto print = [](const auto &x) { std::cout << x << std::endl; };
  auto doubler = [](const auto x) { return x * 2; };
  auto toString = [](const auto x) { return std::to_string(x) + " something"; };
  auto sumAccumulator = [](auto x, auto y) { return x + y; };
  auto newStream = s.map<int>(doubler);
  newStream.map<std::string>(toString).forEach(print);
  auto sum = newStream.reduce(0, sumAccumulator);
  ASSERT_EQ(sum, 30);
  auto limitedSum = newStream.limit(2).reduce(0, sumAccumulator);
  ASSERT_EQ(limitedSum, 6);
}

TEST(StreamTestFixture, ReturnFilterStream) {
  std::vector x{1, 2, 3, 4, 5};
  ListIterator iter(x.begin(), x.end());
  Stream<int> s(iter);
  auto doubler = [](const auto x) { return x * 2; };
  auto sumAccumulator = [](auto x, auto y) { return x + y; };
  auto greaterThan2 = [](auto x) { return x > 4; };
  auto filteredStream = s.map<int>(doubler).filter(greaterThan2);
  EXPECT_THAT(filteredStream.toVector(), ::testing::ElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toSet(), ::testing::UnorderedElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toUnorderedSet(), ::testing::UnorderedElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toList(), ::testing::ElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toDeque(), ::testing::ElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toVector(), ::testing::ElementsAre(6, 8, 10));
  auto sum = filteredStream.reduce(0, sumAccumulator);
  ASSERT_EQ(sum, 24);
}
}// namespace aalbatross::utils::test