#include <aalbatross/utils/collection/streamablevector.h>
#include <aalbatross/utils/streams/collectors.h>
#include <aalbatross/utils/streams/stream.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ostream>
namespace aalbatross::utils::test {
enum BlogPostType {
  NEWS,
  REVIEW,
  GUIDE
};

struct BlogPost {
  friend std::ostream &operator<<(std::ostream &os, const BlogPost &post) {
    os << "{title: " << post.title << " author: " << post.author << " type: " << post.type << " likes: " << post.likes << "}";
    return os;
  }
  std::string title;
  std::string author;
  BlogPostType type;
  int likes;
};

collection::SVector<BlogPost> getPosts() {
  return collection::SVector{
      BlogPost{"post1", "author-1", NEWS, 1},
      BlogPost{"post2", "author-2", NEWS, 12},
      BlogPost{"post3", "author-3", NEWS, 121},
      BlogPost{"post4", "author-1", REVIEW, 11},
      BlogPost{"post5", "author-2", REVIEW, 143},
      BlogPost{"post6", "author-2", GUIDE, 15},
      BlogPost{"post7", "author-2", NEWS, 10},
      BlogPost{"post8", "author-1", NEWS, 90},
      BlogPost{"post9", "author-3", GUIDE, 1004},
      BlogPost{"post10", "author-1", REVIEW, 1905}};
}

struct PostCountTitles {
  friend std::ostream &operator<<(std::ostream &os, const PostCountTitles &titles) {
    os << "postCount: " << titles.postCount << " titles: " << titles.titles;
    return os;
  }
  long postCount;
  std::string titles;
};

TEST(GroupingByFixture, GroupingBySingleColumn) {
  auto dataset = getPosts();
  auto groupedBy = dataset.stream().collect(streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.type; }));
  EXPECT_EQ(groupedBy[GUIDE].size(), 2);
  EXPECT_EQ(groupedBy[REVIEW].size(), 3);
  EXPECT_EQ(groupedBy[NEWS].size(), 5);
}

TEST(GroupingByFixture, GroupingBySingleColumnModifiedKeyType) {
  auto dataset = getPosts();

  auto groupedBy = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return std::pair{post.type, post.author}; }));

  EXPECT_EQ(groupedBy.size(), 7);

  auto groupedBy2 = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return std::tuple{post.type, post.author, post.likes}; }));

  EXPECT_EQ(groupedBy2.size(), 10);
}

TEST(GroupingByFixture, GroupingBySingleColumnModifiedValueType) {
  auto dataset = getPosts();

  auto comparator = [](auto a1, auto a2) {
    return a1.likes < a2.likes;
  };

  auto groupedBy = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.type; },
                                                streams::Collectors::toSet<BlogPost, decltype(comparator)>(comparator)));

  EXPECT_EQ(groupedBy.at(GUIDE).size(), 2);
  EXPECT_EQ(groupedBy.at(REVIEW).size(), 3);
  EXPECT_EQ(groupedBy.at(NEWS).size(), 5);
}

TEST(GroupingByFixture, GroupingByMultipleFields) {
  auto dataset = getPosts();

  auto groupedBy = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.author; },
                                                streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.type; })));

  EXPECT_EQ(3, groupedBy.size());
}

TEST(GroupingByFixture, AggFromGroupedResult) {
  auto dataset = getPosts();

  auto groupedBy = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.type; },
                                                streams::Collectors::averaging([](auto post) { return post.likes; })));

  auto EPSILON = std::numeric_limits<double>::epsilon();
  EXPECT_TRUE((groupedBy[NEWS] - 46.8) <= EPSILON);
  EXPECT_TRUE((groupedBy[GUIDE] - 509.5) <= EPSILON);

  auto groupedBy2 = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.type; },
                                                streams::Collectors::summingLong([](auto post) { return post.likes; })));

  EXPECT_EQ(groupedBy2[NEWS], 234);
  EXPECT_EQ(groupedBy2[REVIEW], 2059);
  EXPECT_EQ(groupedBy2[GUIDE], 1019);
}

TEST(GroupingByFixture, MaxMinFromGroupedResult) {
  auto dataset = getPosts();

  auto groupedBy = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.type; },
                                                streams::Collectors::maxBy<BlogPost>([](auto post1, auto post2) { return post2.likes - post1.likes; })));

  EXPECT_EQ(groupedBy[NEWS].value().likes, 90);
  EXPECT_EQ(groupedBy[REVIEW].value().likes, 1905);
  EXPECT_EQ(groupedBy[GUIDE].value().likes, 1004);
}

TEST(GroupingByFixture, MappingGroupedResultToDifferentType) {
  auto dataset = getPosts();

  auto groupedBy = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.type; },
                                                streams::Collectors::mapping([](auto post) { return post.title; }, streams::Collectors::joining(", ", "Post titles: [", "]"))));

  EXPECT_STREQ(groupedBy[NEWS].c_str(), "Post titles: [post1, post2, post3, post7, post8]");
  EXPECT_STREQ(groupedBy[REVIEW].c_str(), "Post titles: [post4, post5, post10]");
  EXPECT_EQ(groupedBy[GUIDE], "Post titles: [post6, post9]");
}

TEST(GroupingByFixture, AggregatingMultipleAttributesGroupedResult) {
  auto dataset = getPosts();

  auto groupedBy = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.author; },
                                                streams::Collectors::collectingAndThen(streams::Collectors::toVector<BlogPost>(), [](auto posts) {
                                                  iterators::ListIterator iterator(posts.begin(), posts.end());
                                                  streams::Stream<BlogPost> postStream(iterator);
                                                  long count = postStream
                                                                   .map([](auto post) { return post.title; })
                                                                   .collect(streams::Collectors::counting());
                                                  std::string titles = postStream
                                                                           .map([](auto post) { return post.title; })
                                                                           .collect(streams::Collectors::joining(" : "));

                                                  return PostCountTitles{count, titles};
                                                })));

  EXPECT_EQ(groupedBy["author-1"].postCount, 4);
  EXPECT_EQ(groupedBy["author-2"].postCount, 4);
  EXPECT_EQ(groupedBy["author-3"].postCount, 2);
}

}// namespace aalbatross::utils::test
