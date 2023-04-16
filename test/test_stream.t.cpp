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
  auto newStream = s.map(doubler);
  newStream.map(toString).forEach(print);
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
  auto filteredStream = s.map(doubler).filter(greaterThan2);
  EXPECT_THAT(filteredStream.toVector(), ::testing::ElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toSet(), ::testing::UnorderedElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toUnorderedSet(), ::testing::UnorderedElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toList(), ::testing::ElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toDeque(), ::testing::ElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toVector(), ::testing::ElementsAre(6, 8, 10));
  auto sum = filteredStream.reduce(0, sumAccumulator);
  ASSERT_EQ(sum, 24);
}

TEST(StreamTestFixture, ReturnSortedStream) {
  std::vector x{121, 12, 123, 41, 59};
  ListIterator iter(x.begin(), x.end());
  Stream<int> s(iter);
  EXPECT_THAT(s.sorted(std::greater<int>()).toVector(), ::testing::ElementsAre(123, 121, 59, 41, 12));
  EXPECT_THAT(s.sorted(std::less<int>()).toVector(), ::testing::ElementsAre(12, 41, 59, 121, 123));
}

TEST(StreamTestFixture, ReturnDistinctStream) {
  std::vector x{1, 2, 3, 4, 5, 2, 3, 4, 5};
  ListIterator iter(x.begin(), x.end());
  Stream<int> s(iter);
  EXPECT_THAT(s.distinct().toVector(), ::testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(StreamTestFixture, ReturnSkipStream) {
  std::vector x{1, 2, 3, 4, 5, 6, 7};
  ListIterator iter(x.begin(), x.end());
  Stream<int> s(iter);
  EXPECT_THAT(s.skip(2).toVector(), ::testing::ElementsAre(3, 4, 5, 6, 7));
}

TEST(StreamTestFixture, ReturnMaxMinSumStream) {
  std::vector x{21, 20, 10, 16, 40, 50};
  ListIterator iter(x.begin(), x.end());
  Stream<int> s(iter);
  EXPECT_EQ(s.sum(), 157);
  EXPECT_THAT(s.reverse().toVector(), ::testing::ElementsAre(50, 40, 16, 10, 20, 21));
  EXPECT_EQ(s.max().value(), 50);
  EXPECT_EQ(s.min().value(), 10);
}

TEST(StreamTestFixture, ReturnMaxMinStream) {
  std::vector<int> x{};
  ListIterator iter(x.begin(), x.end());
  Stream<int> s(iter);
  EXPECT_EQ(s.sum(), 0);
  EXPECT_THAT(s.toVector(), ::testing::ElementsAre());
  EXPECT_THAT(s.reverse().toVector(), ::testing::ElementsAre());
  EXPECT_FALSE(s.max().has_value());
  EXPECT_FALSE(s.min().has_value());
}
}// namespace aalbatross::utils::test