#include <aalbatross/utils/listiterator.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace aalbatross::utils::test {
TEST(ListIteratorFixture, ReturnListOfValues) {
  std::vector x{1, 2, 3, 4, 5};
  ListIterator iter(x.begin(), x.end());

  std::vector<int> out;
  while (iter.hasNext()) {
    out.push_back(iter.next());
  }

  ASSERT_EQ(out.size(), x.size());
  EXPECT_THAT(out, ::testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(ListIteratorFixture, ReturnListOfStrings) {
  std::vector x{"one", "two", "three", "four", "five"};
  ListIterator iter(x.begin(), x.end());

  std::vector<std::string> out;
  while (iter.hasNext()) {
    out.push_back(iter.next());
  }

  ASSERT_EQ(out.size(), x.size());
  EXPECT_THAT(out,
              ::testing::ElementsAre("one", "two", "three", "four", "five"));
}

TEST(ListIteratorFixture, ReturnListOfStringsWithReplay) {
  std::vector x{"one", "two", "three", "four", "five"};
  ListIterator iter(x.begin(), x.end());

  std::vector<std::string> out;
  while (iter.hasNext()) {
    out.push_back(iter.next());
  }

  iter.reset();
  std::vector<std::string> out1;
  while (iter.hasNext()) {
    out1.push_back(iter.next());
  }

  ASSERT_EQ(out.size(), x.size());
  ASSERT_EQ(out1.size(), x.size());

  EXPECT_THAT(out,
              ::testing::ElementsAre("one", "two", "three", "four", "five"));
  EXPECT_THAT(out1,
              ::testing::ElementsAre("one", "two", "three", "four", "five"));
}

TEST(ListIteratorFixture, ReturnListOfArraysWithReplay) {
  std::array<std::string, 5> x{"one", "two", "three", "four", "five"};
  ListIterator iter(x.begin(), x.end());

  std::vector<std::string> out;
  while (iter.hasNext()) {
    out.emplace_back(iter.next());
  }

  iter.reset();
  std::vector<std::string> out1;
  while (iter.hasNext()) {
    out1.emplace_back(iter.next());
  }

  ASSERT_EQ(out.size(), x.size());
  ASSERT_EQ(out1.size(), x.size());

  EXPECT_THAT(out,
              ::testing::ElementsAre("one", "two", "three", "four", "five"));
  EXPECT_THAT(out1,
              ::testing::ElementsAre("one", "two", "three", "four", "five"));
}

TEST(ListIteratorFixture, ReturnSetWithReplay) {
  std::set<std::string> x{"one", "two", "three", "four", "five"};
  ListIterator iter(x.begin(), x.end());

  std::vector<std::string> out;
  while (iter.hasNext()) {
    out.push_back(iter.next());
  }

  iter.reset();
  std::vector<std::string> out1;
  while (iter.hasNext()) {
    out1.push_back(iter.next());
  }

  ASSERT_EQ(out.size(), x.size());
  ASSERT_EQ(out1.size(), x.size());

  EXPECT_THAT(out,
              ::testing::UnorderedElementsAre("one", "two", "three", "four", "five"));
  EXPECT_THAT(out1,
              ::testing::UnorderedElementsAre("one", "two", "three", "four", "five"));
}

TEST(ListIteratorFixture, ReturnEmptyListIterator) {
  std::set<std::string> x{};
  ListIterator iter(x.begin(), x.end());

  std::vector<std::string> out;
  while (iter.hasNext()) {
    out.push_back(iter.next());
  }

  ASSERT_EQ(out.size(), x.size());

  EXPECT_THAT(out,
              ::testing::UnorderedElementsAre());
}
}// namespace aalbatross::utils::test