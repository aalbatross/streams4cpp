#include <aalbatross/utils/streams/collectors.h>

#include <aalbatross/utils/collection/streamablevector.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace aalbatross::utils::test {

struct AType {
  int a;
  char b;
  std::string c;
};

TEST(CollectorFixtureTest, AveragingDoubleTest) {
  std::vector vector{12, 2, 13, 4, 5};
  std::vector pods{
      AType{23, 'a', "xyz"},
      AType{45, 'b', "ayz"},
      AType{69, 'c', "uyz"},
      AType{13, 'a', "byz"},
  };
  auto collector1 = streams::Collectors::averaging([](auto element) { return element * 1.0; });
  EXPECT_EQ(7.2, collector1.apply(vector));

  auto collector2 = streams::Collectors::averaging([](auto pod) { return pod.a; });
  EXPECT_EQ(37.5, collector2.apply(pods));
}

TEST(CollectorFixtureTest, CountingTest) {
  std::vector vector{12, 2, 13, 4, 5};
  std::vector pods{
      AType{23, 'a', "xyz"},
      AType{45, 'b', "ayz"},
      AType{69, 'c', "uyz"},
      AType{13, 'a', "byz"},
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

TEST(CollectorFixtureTest, GroupingByTest) {
  std::vector vector{12, 12, 13, 13, 5, 4, 5, 5, 5, 5, 4};

  auto collector = streams::Collectors::groupingBy<int>([](auto count) { return std::to_string(count); });
  auto result = collector.apply(vector);

  EXPECT_EQ(4, result.size());
  EXPECT_EQ(5, result["5"].size());
  EXPECT_EQ(2, result["13"].size());
  EXPECT_EQ(2, result["12"].size());
  EXPECT_EQ(2, result["4"].size());
}

TEST(CollectorFixtureTest, GroupingByWithCollectorTest) {
  std::vector vector{12, 12, 13, 13, 5, 4, 5, 5, 5, 5, 4};

  auto collector = streams::Collectors::groupingBy<int>(
      [](auto count) { return std::to_string(count); }, streams::Collectors::counting());
  auto result = collector.apply(vector);

  EXPECT_EQ(4, result.size());
  EXPECT_EQ(5, result["5"]);
  EXPECT_EQ(2, result["13"]);
  EXPECT_EQ(2, result["12"]);
  EXPECT_EQ(2, result["4"]);
}

TEST(CollectorFixtureTest, JoiningTest) {
  std::vector vector{"apple", "boy", "cat", "dog", "elephant", "fish", "girl"};

  auto collector = streams::Collectors::joining();
  auto result = collector.apply(vector);
  const auto *expected = "apple boy cat dog elephant fish girl";
  EXPECT_STREQ(expected, result.c_str());

  auto collector1 = streams::Collectors::joining(",");
  auto result1 = collector1.apply(vector);
  const auto *expected1 = "apple,boy,cat,dog,elephant,fish,girl";
  EXPECT_STREQ(expected1, result1.c_str());

  auto collector2 = streams::Collectors::joining(", ", "[", "]");
  auto result2 = collector2.apply(vector);
  const auto *expected2 = "[apple, boy, cat, dog, elephant, fish, girl]";
  EXPECT_STREQ(expected2, result2.c_str());
}

TEST(CollectorFixtureTest, MaxMinTest) {
  std::vector vector{12, 2, 13, 4, 5};

  auto collector1 = streams::Collectors::maxBy<int>(std::less<>());
  auto result1 = collector1.apply(vector);
  auto expected1 = 13;
  EXPECT_TRUE(result1.has_value());
  EXPECT_EQ(expected1, result1.value());

  auto collector2 = streams::Collectors::minBy<int>(std::less<>());
  auto result2 = collector2.apply(vector);
  auto expected2 = 2;
  EXPECT_TRUE(result2.has_value());
  EXPECT_EQ(expected2, result2.value());
}

TEST(CollectorFixtureTest, PartitioningByTest) {
  std::vector data{21, 20, 29, 10, 17, 16, 40, 50};

  auto collector1 = streams::Collectors::partitioningBy<int>([](auto element) { return element % 2 == 0; });
  auto result1 = collector1.apply(data);
  EXPECT_EQ(2, result1.size());
  EXPECT_THAT(result1[true], ::testing::ElementsAre(20, 10, 16, 40, 50));
  EXPECT_THAT(result1[false], ::testing::ElementsAre(21, 29, 17));

  auto collector2 = streams::Collectors::partitioningBy<int>([](auto element) { return element % 2 == 0; }, streams::Collectors::counting());
  auto result2 = collector2.apply(data);
  EXPECT_EQ(2, result2.size());
  EXPECT_EQ(result2[true], 5);
  EXPECT_EQ(result2[false], 3);
}

TEST(CollectorFixtureTest, CollectToCollectionTest) {
  std::vector data{21, 21, 20, 20, 29, 29, 29, 10, 17, 16, 40, 50};

  auto collector1 = streams::Collectors::toSet<int>();
  auto result1 = collector1.apply(data);

  EXPECT_THAT(result1, ::testing::ElementsAre(10, 16, 17, 20, 21, 29, 40, 50));

  auto collector2 = streams::Collectors::toVector<int>();
  auto result2 = collector2.apply(data);

  EXPECT_THAT(result2, ::testing::ElementsAre(21, 21, 20, 20, 29, 29, 29, 10, 17, 16, 40, 50));
}

TEST(CollectorFixtureTest, CollectToCustomContainerTest) {
  std::vector data{21, 21, 20, 20, 29, 29, 29, 10, 17, 16, 40, 50};

  std::unordered_set<int> unorderedSet;

  auto collector1 = streams::Collectors::toContainer(unorderedSet);
  auto result1 = collector1.apply(data);

  EXPECT_THAT(result1, ::testing::UnorderedElementsAre(10, 16, 17, 20, 21, 29, 40, 50));
}

TEST(CollectorFixtureTest, CollectToSummingTest) {
  std::vector data{12, 2, 13, 4, 5};

  auto collector1 = streams::Collectors::summingLong([](int item) { return item; });
  auto result1 = collector1.apply(data);

  EXPECT_EQ(result1, 36);

  auto collector2 = streams::Collectors::summingDouble([](int item) { return item * 1.0; });
  auto result2 = collector2.apply(data);

  EXPECT_EQ(result2, 36.0);
}

TEST(CollectorFixtureTest, CollectToMapTest1) {
  std::vector pods{
      AType{23, 'a', "xyz"},
      AType{45, 'b', "ayz"},
      AType{69, 'c', "uyz"},
      AType{13, 'a', "byz"},
  };
  auto collector1 = streams::Collectors::toMap<AType>([](auto pod) { return pod.a; }, [](auto pod) { return pod.c; });
  auto result1 = collector1.apply(pods);

  EXPECT_STREQ(result1[23].c_str(), "xyz");
  EXPECT_STREQ(result1[45].c_str(), "ayz");
  EXPECT_STREQ(result1[69].c_str(), "uyz");
  EXPECT_STREQ(result1[13].c_str(), "byz");
}

TEST(CollectorFixtureTest, CollectToMapTest2) {
  std::vector pods{
      AType{23, 'a', "xyz"},
      AType{45, 'b', "ayz"},
      AType{69, 'c', "uyz"},
      AType{13, 'a', "byz"},
  };

  auto collector2 = streams::Collectors::toMap<AType>([](auto pod) { return pod.b; }, [](auto pod) { return pod.c; }, [](auto a_1, auto a_2) { return a_1 + ", " + a_2; });
  auto result2 = collector2.apply(pods);
  EXPECT_STREQ(result2['a'].c_str(), ", xyz, byz");
  EXPECT_STREQ(result2['b'].c_str(), ", ayz");
  EXPECT_STREQ(result2['c'].c_str(), ", uyz");
  EXPECT_EQ(3, result2.size());
}

TEST(CollectorFixtureTest, CollectReducingTest) {
  std::vector pods{
      AType{23, 'a', "xyz"},
      AType{45, 'b', "ayz"},
      AType{69, 'c', "uyz"},
      AType{13, 'a', "byz"},
  };
  auto collector1 = streams::Collectors::reducing<AType>([](auto pod1, auto pod2) { return AType{pod1.a + pod2.a, pod2.b, pod1.c + "," + pod2.c}; });
  auto result1 = collector1.apply(pods);
  EXPECT_EQ(150, result1.a);
  EXPECT_EQ(((char) 97), result1.b);
  EXPECT_STREQ(",xyz,ayz,uyz,byz", result1.c.c_str());

  auto collector2 = streams::Collectors::reducing<AType>(AType{256, 'd', "def"}, [](auto pod1, auto pod2) { return AType{pod1.a + pod2.a, pod2.b, pod1.c + "," + pod2.c}; });
  auto result2 = collector2.apply(pods);
  EXPECT_EQ(406, result2.a);
  EXPECT_EQ(((char) 97), result2.b);
  EXPECT_STREQ("def,xyz,ayz,uyz,byz", result2.c.c_str());
}
}// namespace aalbatross::utils::test
