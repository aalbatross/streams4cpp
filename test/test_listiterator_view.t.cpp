#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <listiterator_view.h>

namespace aalbatross::util::test {
TEST(ListIteratorViewFixture, ReturnListOfValues) {
  std::vector x{1, 2, 3, 4, 5};
  ListIteratorView iter(x);

  std::vector<int> out;
  while (iter.hasNext()) {
    out.push_back(iter.next());
  }

  ASSERT_EQ(out.size(), x.size());
  EXPECT_THAT(out, ::testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(ListIteratorViewFixture, ReturnListOfStrings) {
  std::vector x{"one", "two", "three", "four", "five"};
  ListIteratorView iter(x);

  std::vector<std::string> out;
  while (iter.hasNext()) {
    out.push_back(iter.next());
  }

  ASSERT_EQ(out.size(), x.size());
  EXPECT_THAT(out,
              ::testing::ElementsAre("one", "two", "three", "four", "five"));
}

TEST(ListIteratorViewFixture, ReturnListOfStringsWithReplay) {
  std::vector x{"one", "two", "three", "four", "five"};
  ListIteratorView iter(x);

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

TEST(ListIteratorViewFixture, ReturnListOfArraysWithReplay) {
  std::array<std::string, 5> x{"one", "two", "three", "four", "five"};
  ListIteratorView iter(x);

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

TEST(ListIteratorViewFixture, ReturnSetWithReplay) {
  std::set<std::string> x{"one", "two", "three", "four", "five"};
  ListIteratorView iter(x);

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

auto getIterator() {
  std::set<std::string> x{"one", "two", "three", "four", "five"};
  return ListIteratorView(x);
}

TEST(ListIteratorViewFixture, ReturnViewableIterator) {
  std::set<std::string> x{"one", "two", "three", "four", "five"};
  auto iter = getIterator();

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

};// namespace aalbatross::util::test