#include <aalbatross/utils/collection/streamabledeque.h>
#include <aalbatross/utils/collection/streamablelist.h>
#include <aalbatross/utils/collection/streamablemap.h>
#include <aalbatross/utils/collection/streamableset.h>
#include <aalbatross/utils/collection/streamableunorderedmap.h>
#include <aalbatross/utils/collection/streamableunorderedset.h>
#include <aalbatross/utils/collection/streamablevector.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace aalbatross::utils::test {

const auto doubler = [](const auto element) { return element * 2; };
const auto sumAccumulator = [](auto x_1, auto y_1) { return x_1 + y_1; };
const auto greaterThan4 = [](auto number) { return number > 4; };
const auto print = [](const auto &element) { std::cout << element << std::endl; };
const auto toString = [](const auto element) { return std::to_string(element) + " something"; };

TEST(SVectorTestFixture, ReturnTransformedStream) {
  collection::SVector svector{1, 2, 3, 4, 5};
  auto stream = svector.stream().map(doubler).filter(greaterThan4);
  EXPECT_THAT(stream.toVector(), ::testing::ElementsAre(6, 8, 10));
  std::vector<int> values;
  auto &iter = svector.iterator();
  while (iter.hasNext()) {
    values.push_back(iter.next().value());
  }
  EXPECT_THAT(values, ::testing::ElementsAre(1, 2, 3, 4, 5));
  collection::SVector<int> svector1;
  EXPECT_TRUE(svector1.empty());
  EXPECT_THAT(svector1.stream().toVector(), ::testing::ElementsAre());
}

TEST(SListTestFixture, ReturnTransformedStream) {
  collection::SList slist{1, 2, 3, 4, 5};
  auto stream = slist.stream().map(doubler).filter(greaterThan4);
  EXPECT_THAT(stream.toVector(), ::testing::ElementsAre(6, 8, 10));
  std::vector<int> values;
  auto &iter = slist.iterator();
  while (iter.hasNext()) {
    values.push_back(iter.next().value());
  }
  EXPECT_THAT(values, ::testing::ElementsAre(1, 2, 3, 4, 5));
  collection::SList<int> slist1;
  EXPECT_TRUE(slist1.empty());
  EXPECT_THAT(slist1.stream().toVector(), ::testing::ElementsAre());
}

TEST(SDequeTestFixture, ReturnTransformedStream) {
  collection::SDeque sdq{1, 2, 3, 4, 5};
  auto stream = sdq.stream().map(doubler).filter(greaterThan4);
  EXPECT_THAT(stream.toVector(), ::testing::ElementsAre(6, 8, 10));
  std::vector<int> values;
  auto &iter = sdq.iterator();
  while (iter.hasNext()) {
    values.push_back(iter.next().value());
  }
  EXPECT_THAT(values, ::testing::ElementsAre(1, 2, 3, 4, 5));
  collection::SDeque<int> sdq1;
  EXPECT_TRUE(sdq1.empty());
  EXPECT_THAT(sdq1.stream().toVector(), ::testing::ElementsAre());
}

TEST(SSetTestFixture, ReturnTransformedStream) {
  collection::SSet sset{1, 2, 3, 4, 5};
  auto stream = sset.stream().map(doubler).filter(greaterThan4);
  EXPECT_THAT(stream.toVector(), ::testing::ElementsAre(6, 8, 10));
  std::vector<int> values;
  auto &iter = sset.iterator();
  while (iter.hasNext()) {
    values.push_back(iter.next().value());
  }
  EXPECT_THAT(values, ::testing::ElementsAre(1, 2, 3, 4, 5));
  collection::SSet<int> sset1;
  EXPECT_TRUE(sset1.empty());
  EXPECT_THAT(sset1.stream().toVector(), ::testing::ElementsAre());
}

TEST(SUSetTestFixture, ReturnTransformedStream) {
  collection::SUSet sset{1, 2, 3, 4, 5};
  auto stream = sset.stream().map(doubler).filter(greaterThan4);
  EXPECT_THAT(stream.toVector(), ::testing::UnorderedElementsAre(6, 8, 10));
  std::vector<int> values;
  auto &iter = sset.iterator();
  while (iter.hasNext()) {
    values.push_back(iter.next().value());
  }
  EXPECT_THAT(values, ::testing::UnorderedElementsAre(1, 2, 3, 4, 5));
  collection::SUSet<int> sset1;
  EXPECT_TRUE(sset1.empty());
  EXPECT_THAT(sset1.stream().toVector(), ::testing::ElementsAre());
}

TEST(SMapTestFixture, ReturnTransformedStream) {
  collection::SMap<int, std::string> smap{{1, "one"}, {2, "two"}, {3, "three"}};
  auto stream = smap.stream().map([](auto element) { return element.second; });
  EXPECT_THAT(stream.toVector(), ::testing::ElementsAre("one", "two", "three"));
  std::vector<int> values;
  auto &iter = smap.iterator();
  while (iter.hasNext()) {
    values.push_back(iter.next().value().first);
  }
  EXPECT_THAT(values, ::testing::ElementsAre(1, 2, 3));
  collection::SMap<int, std::string> smap1;
  EXPECT_TRUE(smap1.empty());
  EXPECT_THAT(smap1.stream().toVector(), ::testing::ElementsAre());
}

TEST(SUMapTestFixture, ReturnTransformedStream) {
  collection::SUMap<int, std::string> smap{{1, "one"}, {2, "two"}, {3, "three"}};
  auto stream = smap.stream().map([](auto element) { return element.second; });
  EXPECT_THAT(stream.toVector(), ::testing::UnorderedElementsAre("one", "two", "three"));
  std::vector<int> values;
  auto &iter = smap.iterator();
  while (iter.hasNext()) {
    values.push_back(iter.next().value().first);
  }
  EXPECT_THAT(values, ::testing::UnorderedElementsAre(1, 2, 3));
  collection::SUMap<int, std::string> smap1;
  EXPECT_TRUE(smap1.empty());
  EXPECT_THAT(smap1.stream().toVector(), ::testing::ElementsAre());
}
}// namespace aalbatross::utils::test