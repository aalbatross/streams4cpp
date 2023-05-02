#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"
#include <aalbatross/utils/iterators/listiterator.h>
#include <aalbatross/utils/streams/stream.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
namespace aalbatross::utils::test {
using namespace streams;
using namespace iterators;
const auto doubler = [](const auto element) { return element * 2; };
const auto sumAccumulator = [](auto x_1, auto y_1) { return x_1 + y_1; };
const auto greaterThan4 = [](auto number) { return number > 4; };
const auto print = [](const auto &element) { std::cout << element << std::endl; };
const auto toString = [](const auto element) { return std::to_string(element) + " something"; };

TEST(StreamTestFixture, ReturnTransformedStream) {
  std::vector data{1, 2, 3, 4, 5};
  Stream<int> stream(data.begin(), data.end());
  auto newStream = stream.map(doubler);
  newStream.map(toString).forEach(print);
  auto sum = newStream.reduce(0, sumAccumulator);
  ASSERT_EQ(sum, 30);
  auto limitedSum = newStream.limit(2).reduce(0, sumAccumulator);
  ASSERT_EQ(limitedSum, 6);
}

TEST(StreamTestFixture, ReturnFilterStream) {
  std::vector data{1, 2, 3, 4, 5};
  Stream<int> stream(data.begin(), data.end());
  auto filteredStream = stream.map(doubler).filter(greaterThan4);
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
  std::vector data{121, 12, 123, 41, 59};
  Stream<int> stream(data.begin(), data.end());
  EXPECT_THAT(stream.sorted(std::greater<>()).toVector(), ::testing::ElementsAre(123, 121, 59, 41, 12));
  EXPECT_THAT(stream.sorted(std::less<>()).toVector(), ::testing::ElementsAre(12, 41, 59, 121, 123));
}

TEST(StreamTestFixture, ReturnDistinctStream) {
  std::vector data{1, 2, 3, 4, 5, 2, 3, 4, 5};
  Stream<int> stream(data.begin(), data.end());
  EXPECT_THAT(stream.distinct().toVector(), ::testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(StreamTestFixture, ReturnSkipStream) {
  std::vector data{1, 2, 3, 4, 5, 6, 7};
  Stream<int> stream(data.begin(), data.end());
  EXPECT_THAT(stream.skip(2).toVector(), ::testing::ElementsAre(3, 4, 5, 6, 7));
}

TEST(StreamTestFixture, ReturnMaxMinSumStream) {
  std::vector data{21, 20, 10, 16, 40, 50};
  Stream<int> stream(data.begin(), data.end());
  EXPECT_EQ(stream.sum(), 157);
  EXPECT_THAT(stream.reverse().toVector(), ::testing::ElementsAre(50, 40, 16, 10, 20, 21));
  EXPECT_TRUE(stream.max().has_value());
  EXPECT_TRUE(stream.min().has_value());
  EXPECT_EQ(stream.max().value(), 50);
  EXPECT_EQ(stream.min().value(), 10);
}

TEST(StreamTestFixture, ReturnMaxMinStream) {
  std::vector<int> data{};
  Stream<int> stream(data.begin(), data.end());
  EXPECT_EQ(stream.sum(), 0);
  EXPECT_THAT(stream.toVector(), ::testing::ElementsAre());
  EXPECT_THAT(stream.reverse().toVector(), ::testing::ElementsAre());
  EXPECT_FALSE(stream.max().has_value());
  EXPECT_FALSE(stream.min().has_value());
}

TEST(StreamTestFixture, ReturnMatchStream) {
  std::vector data{21, 20, 10, 16, 40, 50};
  Stream<int> stream(data.begin(), data.end());
  EXPECT_TRUE(stream.allMatch([](auto num) { return num >= 10; }));
  EXPECT_FALSE(stream.allMatch([](auto num) { return num >= 20; }));
  EXPECT_TRUE(stream.anyMatch([](auto num) { return num >= 20; }));
  EXPECT_FALSE(stream.anyMatch([](auto num) { return num <= 0; }));
  EXPECT_TRUE(stream.noneMatch([](auto num) { return num <= 0; }));
  EXPECT_FALSE(stream.noneMatch([](auto num) { return num >= 0; }));
}

TEST(StreamTestFixture, ReturnFindFirstStream) {
  std::vector data{21, 20, 10, 16, 40, 50};
  Stream<int> stream(data.begin(), data.end());
  EXPECT_TRUE(stream.head().has_value());
  EXPECT_TRUE(stream.tail().has_value());
  EXPECT_EQ(stream.head().value(), 21);
  EXPECT_EQ(stream.tail().value(), 50);
  EXPECT_EQ(stream.find([](auto num) { return num % 25 == 0; }).value(), 50);
}

TEST(StreamTestFixture, ReturnGroupedByStream) {
  std::vector data{21, 20, 29, 10, 17, 16, 40, 50};
  Stream<int> stream(data.begin(), data.end());
  auto result = stream.groupedBy([](auto number) { return number % 2 == 0 ? "even" : "odd"; });
  EXPECT_EQ(2, result.size());
  EXPECT_THAT(result["even"], ::testing::ElementsAre(20, 10, 16, 40, 50));
  EXPECT_THAT(result["odd"], ::testing::ElementsAre(21, 29, 17));
}

Stream<int>::View getStream() {
  std::vector data{21, 20, 29, 10, 17, 16, 40, 50};
  Stream<int> stream(data.begin(), data.end());
  return stream.view();
}

TEST(StreamTestFixture, ReturnStreamViewToStreamTest) {
  auto stream = getStream();
  auto result = stream.stream().groupedBy([](auto number) { return number % 2 == 0 ? "even" : "odd"; });
  EXPECT_EQ(2, result.size());
  EXPECT_THAT(result["even"], ::testing::ElementsAre(20, 10, 16, 40, 50));
  EXPECT_THAT(result["odd"], ::testing::ElementsAre(21, 29, 17));
}
}// namespace aalbatross::utils::test
#pragma clang diagnostic pop