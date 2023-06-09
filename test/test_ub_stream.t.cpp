#include <aalbatross/utils/streams/collectors.h>
#include <aalbatross/utils/streams/ub_stream.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
namespace aalbatross::utils::test {
TEST(UBStreamTestFixture, EmptyProcessorTest) {
  std::vector data{1, 2, 3, 4, 5};
  streams::UBStream<int> stream(data.begin(), data.end());
  EXPECT_THAT(stream.toVector(), ::testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(UBStreamTestFixture, MappersTest) {
  std::vector data{1, 2, 3, 4, 5};
  streams::UBStream<int> stream(data.begin(), data.end());
  auto mappedStream = stream
                          .map([](auto element) { return element + 100; })
                          .map([](auto element) { return std::to_string(element) + " " + std::to_string(1); });
  EXPECT_THAT(mappedStream.toVector(), ::testing::ElementsAre("101 1", "102 1", "103 1", "104 1", "105 1"));
}

TEST(UBStreamTestFixture, FilterTest) {
  std::vector data{1, 2, 3, 4, 5};
  streams::UBStream<int> stream(data.begin(), data.end());
  auto onlyEvenElements = stream
                              .filter([](auto element) { return element % 2 == 0; });
  EXPECT_THAT(onlyEvenElements.toVector(), ::testing::ElementsAre(2, 4));
}

TEST(UBStreamTestFixture, LimitTest) {
  std::vector data{1, 2, 3, 4, 5};
  streams::UBStream<int> stream(data.begin(), data.end());
  auto limitStream = stream.limit(3);
  EXPECT_THAT(limitStream.toVector(), ::testing::ElementsAre(1, 2, 3));

  auto remainingStream = limitStream.map([](auto element) { return element * 2; }).skip(1);
  EXPECT_THAT(remainingStream.toVector(), ::testing::ElementsAre(4, 6));
}

TEST(UBStreamTestFixture, SkipTest) {
  std::vector data{1, 2, 3, 4, 5};
  streams::UBStream<int> stream(data.begin(), data.end());
  auto remainingStream = stream.skip(3);
  EXPECT_THAT(remainingStream.toVector(), ::testing::ElementsAre(4, 5));

  auto remainingStream2 = remainingStream.map([](auto element) { return element * 2; }).skip(1);
  EXPECT_THAT(remainingStream2.toVector(), ::testing::ElementsAre(10));
}

TEST(UBStreamTestFixture, WindowingTest) {
  std::vector data{1, 2, 3, 4, 5};
  streams::UBStream<int> stream(data.begin(), data.end());
  auto windowedStream = stream.sliding(2);
  auto windows = windowedStream.toVector();
  EXPECT_EQ(4, windows.size());
  EXPECT_THAT(windows[0], ::testing::ElementsAre(1, 2));
  EXPECT_THAT(windows[1], ::testing::ElementsAre(2, 3));
  EXPECT_THAT(windows[2], ::testing::ElementsAre(3, 4));
  EXPECT_THAT(windows[3], ::testing::ElementsAre(4, 5));
}

TEST(UBStreamTestFixture, FixedWindowingTest) {
  std::vector data{1, 2, 3, 4, 5};
  streams::UBStream<int> stream(data.begin(), data.end());
  auto windowedStream = stream.fixed(2);
  auto windows = windowedStream.toVector();
  EXPECT_EQ(2, windows.size());
  EXPECT_THAT(windows[0], ::testing::ElementsAre(1, 2));
  EXPECT_THAT(windows[1], ::testing::ElementsAre(3, 4));
}

TEST(UBStreamTestFixture, FlatMapTest) {
  std::vector data{1, 2, 3, 4, 5, 6};
  streams::UBStream<int> stream(data.begin(), data.end());
  auto flatMappedStream = stream.fixed(2).flatten([](auto element) { return element; });
  auto elements = flatMappedStream.toVector();

  EXPECT_THAT(elements, ::testing::ElementsAre(1, 2, 3, 4, 5, 6));
}

TEST(UBStreamTestFixture, MapWithFlatMapTest) {
  std::vector data{1, 2, 3, 4, 5, 6};
  streams::UBStream<int> stream(data.begin(), data.end());
  auto flatMappedStream = stream.fixed(2).flatten([](auto element) { return element; }).map([](auto element) { return element * 10; });
  auto elements = flatMappedStream.toVector();

  EXPECT_THAT(elements, ::testing::ElementsAre(10, 20, 30, 40, 50, 60));
}

TEST(UBStreamTestFixture, OnStreamAggregationTest) {
  std::vector data{1, 2, 3, 4, 5, 6};
  streams::UBStream<int> stream(data.begin(), data.end());
  auto flatMappedStream = stream.sliding(2).map([](auto element) { return element.stream().sum(); });
  auto elements = flatMappedStream.toVector();
  EXPECT_THAT(elements, ::testing::ElementsAre(3, 5, 7, 9, 11));
}

TEST(UBStreamTestFixture, MovingAverageTest) {
  std::vector data{110.0, 213.90, 311.69, 412.23, 512.1, 610.03, 1000.0, 2102.12};
  streams::UBStream<double> stream(data.begin(), data.end());
  auto movingAverageStream = stream
                                 .sliding(2)
                                 .map([](auto element) { return element.stream().collect(streams::Collectors::averaging()); });
  auto elements = movingAverageStream.toVector();
  using namespace ::testing;
  EXPECT_THAT(elements, ::testing::ElementsAre(DoubleEq(161.95), DoubleEq(262.795), DoubleEq(361.96), DoubleEq(462.165), DoubleEq(561.065), DoubleEq(805.015), DoubleEq(1551.06)));
}

}// namespace aalbatross::utils::test