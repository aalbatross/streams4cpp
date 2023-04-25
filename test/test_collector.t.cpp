#include <aalbatross/utils/streams/collectors.h>

#include <aalbatross/utils/collection/streamablevector.h>

#include <gtest/gtest.h>

namespace aalbatross::utils::test {

struct A {
  int a;
  char b;
  std::string c;
};

TEST(CollectorFixtureTest, AveragingDoubleTest) {
  std::vector vector{12, 2, 13, 4, 5};
  std::vector pods{
      A{23, 'a', "xyz"},
      A{45, 'b', "ayz"},
      A{69, 'c', "uyz"},
      A{13, 'a', "byz"},
  };
  auto collector1 = streams::Collectors::averaging([](auto element) { return element * 1.0; });
  EXPECT_EQ(7.2, collector1.apply(vector));

  auto collector2 = streams::Collectors::averaging([](auto pod) { return pod.a; });
  EXPECT_EQ(37.5, collector2.apply(pods));
}

TEST(CollectorFixtureTest, CountingTest) {
  std::vector vector{12, 2, 13, 4, 5};
  std::vector pods{
      A{23, 'a', "xyz"},
      A{45, 'b', "ayz"},
      A{69, 'c', "uyz"},
      A{13, 'a', "byz"},
  };
  auto collector1 = streams::Collectors::counting();
  EXPECT_EQ(5, collector1.apply(vector));

  auto collector2 = streams::Collectors::counting();
  EXPECT_EQ(4, collector2.apply(pods));
}

TEST(CollectorFixtureTest, CollectingAndThenTest) {
  std::vector vector{12, 2, 13, 4, 5};

  auto collector1 = streams::Collectors::collectingAndThen(streams::Collectors::counting(), [](auto count) { return count * 10.5; });
  EXPECT_EQ(52.5, collector1.apply(vector));
}
template<typename T>
std::ostream &operator<<(std::ostream &ostream, std::vector<T> &elements) {
  ostream << "[";
  for (auto &element : elements) {
    ostream << element << ", ";
  }
  ostream << "]";
  return ostream;
}

TEST(CollectorFixtureTest, GroupingByTest) {
  std::vector vector{12, 12, 13, 13, 5, 4, 5, 5, 5, 5, 4};

  auto collector = streams::Collectors::groupingBy<std::string, int>([](auto count) { return std::to_string(count); });
  auto result = collector.apply(vector);

  std::for_each(result.begin(), result.end(), [](auto element) {
    std::cout << element.first << " " << element.second << std::endl;
  });
  EXPECT_EQ(4, result.size());
  EXPECT_EQ(5, result["5"].size());
  EXPECT_EQ(2, result["13"].size());
  EXPECT_EQ(2, result["12"].size());
  EXPECT_EQ(2, result["4"].size());
}

TEST(CollectorFixtureTest, GroupingByWithCollectorTest) {
  std::vector vector{12, 12, 13, 13, 5, 4, 5, 5, 5, 5, 4};

  auto collector = streams::Collectors::groupingBy<std::string, int>(
      [](auto count) { return std::to_string(count); }, streams::Collectors::counting());
  auto result = collector.apply(vector);

  std::for_each(result.begin(), result.end(), [](auto element) {
    std::cout << element.first << " " << element.second << std::endl;
  });
  EXPECT_EQ(4, result.size());
  EXPECT_EQ(5, result["5"]);
  EXPECT_EQ(2, result["13"]);
  EXPECT_EQ(2, result["12"]);
  EXPECT_EQ(2, result["4"]);
}
}// namespace aalbatross::utils::test
