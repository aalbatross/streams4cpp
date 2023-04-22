#include <aalbatross/utils/listiterator_view.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace aalbatross::utils::test {
TEST(ListIteratorViewFixture, ReturnListOfValues) {
  std::vector vec{1, 2, 3, 4, 5};
  iterators::ListIteratorView iter(vec);

  std::vector<int> out;
  while (iter.hasNext()) {
    out.emplace_back(iter.next());
  }

  ASSERT_EQ(out.size(), vec.size());
  EXPECT_THAT(out, ::testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(ListIteratorViewFixture, ReturnListOfStrings) {
  std::vector vec{"one", "two", "three", "four", "five"};
  iterators::ListIteratorView iter(vec);

  std::vector<std::string> out;
  while (iter.hasNext()) {
    out.emplace_back(iter.next());
  }

  ASSERT_EQ(out.size(), vec.size());
  EXPECT_THAT(out,
              ::testing::ElementsAre("one", "two", "three", "four", "five"));
}

TEST(ListIteratorViewFixture, ReturnListOfStringsWithReplay) {
  std::vector vec{"one", "two", "three", "four", "five"};
  iterators::ListIteratorView iter(vec);

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

TEST(ListIteratorViewFixture, ReturnListOfArraysWithReplay) {
  std::array<std::string, 5> arr{"one", "two", "three", "four", "five"};
  iterators::ListIteratorView iter(arr);

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

TEST(ListIteratorViewFixture, ReturnSetWithReplay) {
  std::set<std::string> set{"one", "two", "three", "four", "five"};
  iterators::ListIteratorView iter(set);

  std::vector<std::string> out;
  while (iter.hasNext()) {
    out.push_back(iter.next());
  }

  iter.reset();
  std::vector<std::string> out1;
  while (iter.hasNext()) {
    out1.push_back(iter.next());
  }

  ASSERT_EQ(out.size(), set.size());
  ASSERT_EQ(out1.size(), set.size());

  EXPECT_THAT(out,
              ::testing::UnorderedElementsAre("one", "two", "three", "four", "five"));
  EXPECT_THAT(out1,
              ::testing::UnorderedElementsAre("one", "two", "three", "four", "five"));
}

auto getIterator() {
  std::set<std::string> set{"one", "two", "three", "four", "five"};
  return iterators::ListIteratorView(set);
}

TEST(ListIteratorViewFixture, ReturnViewableIterator) {
  std::set<std::string> set{"one", "two", "three", "four", "five"};
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

  ASSERT_EQ(out.size(), set.size());
  ASSERT_EQ(out1.size(), set.size());

  EXPECT_THAT(out,
              ::testing::UnorderedElementsAre("one", "two", "three", "four", "five"));
  EXPECT_THAT(out1,
              ::testing::UnorderedElementsAre("one", "two", "three", "four", "five"));
}

TEST(ListIteratorViewFixture, ReturnEmptyListIterator) {
  std::set<std::string> emptySet{};
  iterators::ListIteratorView iter(emptySet);

  std::vector<std::string> out;
  while (iter.hasNext()) {
    out.push_back(iter.next());
  }

  ASSERT_EQ(out.size(), emptySet.size());

  EXPECT_THAT(out,
              ::testing::UnorderedElementsAre());
}

}// namespace aalbatross::utils::test