#include "aalbatross/utils/iterators/listiterator.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace aalbatross::utils::test {
TEST(ListIteratorFixture, ReturnListOfValues) {
  std::vector vec{1, 2, 3, 4, 5};
  iterators::ListIterator iter(vec.begin(), vec.end());

  std::vector<int> out;
  while (iter.hasNext()) {
    out.emplace_back(iter.next());
  }

  ASSERT_EQ(out.size(), vec.size());
  EXPECT_THAT(out, ::testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(ListIteratorFixture, ReturnListOfStrings) {
  std::vector vec{"one", "two", "three", "four", "five"};
  iterators::ListIterator iter(vec.begin(), vec.end());

  std::vector<std::string> out;
  while (iter.hasNext()) {
    out.emplace_back(iter.next());
  }

  ASSERT_EQ(out.size(), vec.size());
  EXPECT_THAT(out,
              ::testing::ElementsAre("one", "two", "three", "four", "five"));
}

TEST(ListIteratorFivecture, ReturnListOfStringsWithReplay) {
  std::vector vec{"one", "two", "three", "four", "five"};
  iterators::ListIterator iter(vec.begin(), vec.end());

  std::vector<std::string> out;
  while (iter.hasNext()) {
    out.emplace_back(iter.next());
  }

  iter.reset();
  std::vector<std::string> out1;
  while (iter.hasNext()) {
    out1.emplace_back(iter.next());
  }

  ASSERT_EQ(out.size(), vec.size());
  ASSERT_EQ(out1.size(), vec.size());

  EXPECT_THAT(out,
              ::testing::ElementsAre("one", "two", "three", "four", "five"));
  EXPECT_THAT(out1,
              ::testing::ElementsAre("one", "two", "three", "four", "five"));
}

TEST(ListIteratorFixture, ReturnListOfArraysWithReplay) {
  std::array<std::string, 5> arr{"one", "two", "three", "four", "five"};
  iterators::ListIterator iter(arr.begin(), arr.end());

  std::vector<std::string> out;
  while (iter.hasNext()) {
    out.emplace_back(iter.next());
  }

  iter.reset();
  std::vector<std::string> out1;
  while (iter.hasNext()) {
    out1.emplace_back(iter.next());
  }

  ASSERT_EQ(out.size(), arr.size());
  ASSERT_EQ(out1.size(), arr.size());

  EXPECT_THAT(out,
              ::testing::ElementsAre("one", "two", "three", "four", "five"));
  EXPECT_THAT(out1,
              ::testing::ElementsAre("one", "two", "three", "four", "five"));
}

TEST(ListIteratorFixture, ReturnSetWithReplay) {
  std::set<std::string> set{"one", "two", "three", "four", "five"};
  iterators::ListIterator iter(set.begin(), set.end());

  std::vector<std::string> out;
  while (iter.hasNext()) {
    out.emplace_back(iter.next());
  }

  iter.reset();
  std::vector<std::string> out1;
  while (iter.hasNext()) {
    out1.emplace_back(iter.next());
  }

  ASSERT_EQ(out.size(), set.size());
  ASSERT_EQ(out1.size(), set.size());

  EXPECT_THAT(out,
              ::testing::UnorderedElementsAre("one", "two", "three", "four", "five"));
  EXPECT_THAT(out1,
              ::testing::UnorderedElementsAre("one", "two", "three", "four", "five"));
}

TEST(ListIteratorFixture, ReturnEmptyListIterator) {
  std::set<std::string> set{};
  iterators::ListIterator iter(set.begin(), set.end());

  std::vector<std::string> out;
  while (iter.hasNext()) {
    out.emplace_back(iter.next());
  }

  ASSERT_EQ(out.size(), set.size());

  EXPECT_THAT(out,
              ::testing::UnorderedElementsAre());
}
}// namespace aalbatross::utils::test