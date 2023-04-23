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
    values.push_back(iter.next());
  }
  EXPECT_THAT(values, ::testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(SListTestFixture, ReturnTransformedStream) {
  collection::SList slist{1, 2, 3, 4, 5};
  auto stream = slist.stream().map(doubler).filter(greaterThan4);
  EXPECT_THAT(stream.toVector(), ::testing::ElementsAre(6, 8, 10));
  std::vector<int> values;
  auto &iter = slist.iterator();
  while (iter.hasNext()) {
    values.push_back(iter.next());
  }
  EXPECT_THAT(values, ::testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(SDequeTestFixture, ReturnTransformedStream) {
  collection::SDeque sdq{1, 2, 3, 4, 5};
  auto stream = sdq.stream().map(doubler).filter(greaterThan4);
  EXPECT_THAT(stream.toVector(), ::testing::ElementsAre(6, 8, 10));
  std::vector<int> values;
  auto &iter = sdq.iterator();
  while (iter.hasNext()) {
    values.push_back(iter.next());
  }
  EXPECT_THAT(values, ::testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(SSetTestFixture, ReturnTransformedStream) {
  collection::SSet sset{1, 2, 3, 4, 5};
  auto stream = sset.stream().map(doubler).filter(greaterThan4);
  EXPECT_THAT(stream.toVector(), ::testing::ElementsAre(6, 8, 10));
  std::vector<int> values;
  auto &iter = sset.iterator();
  while (iter.hasNext()) {
    values.push_back(iter.next());
  }
  EXPECT_THAT(values, ::testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(SMapTestFixture, ReturnTransformedStream) {
  collection::SMap<int, std::string> smap{{1, "one"}, {2, "two"}, {3, "three"}};
  auto stream = smap.stream().map([](auto e) { return e.second; });
  EXPECT_THAT(stream.toVector(), ::testing::ElementsAre("one", "two", "three"));
  std::vector<int> values;
  auto &iter = smap.iterator();
  while (iter.hasNext()) {
    values.push_back(iter.next().first);
  }
  EXPECT_THAT(values, ::testing::ElementsAre(1, 2, 3));
}

TEST(SUMapTestFixture, ReturnTransformedStream) {
  collection::SUMap<int, std::string> smap{{1, "one"}, {2, "two"}, {3, "three"}};
  auto stream = smap.stream().map([](auto e) { return e.second; });
  EXPECT_THAT(stream.toVector(), ::testing::UnorderedElementsAre("one", "two", "three"));
  std::vector<int> values;
  auto &iter = smap.iterator();
  while (iter.hasNext()) {
    values.push_back(iter.next().first);
  }
  EXPECT_THAT(values, ::testing::UnorderedElementsAre(1, 2, 3));
}
}// namespace aalbatross::utils::test