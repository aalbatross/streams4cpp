## Introduction to Collectors
List of some common and useful reduction operations on stream together is called Collector here. This guide will show by examples how to apply them to the Stream.

## Illustrative Examples
Let's define BlogPost as a structure to show examples on this type.

```c++
enum BlogPostType {
  NEWS,
  REVIEW,
  GUIDE
};

struct BlogPost {
  std::string title;
  std::string author;
  BlogPostType type;
  int likes;
};
```

### Group by examples

#### Group by on Single Column
Let's group by stream of blogposts with respect to its types.
```c++
  map<BlogPostType, vector<BlogPost>> groupedBy = dataset.stream().collect(streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.type; }));
```

#### Group by on Single Column with modified key type
- Let's group by stream of blogposts with respect to a tuple of blog author and blog post type published by author.
```c++
map<pair<BlogPostType, string>, vector<BlogPost>> groupedBy = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return std::pair{post.type, post.author}; }));
```
- Let's group by stream of blogposts with respect to a tuple of blog author and blog post type published by author and likes received on the blog.
```c++
map<tuple<BlogPostType, string, int>, vector<BlogPost>> groupedBy = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return std::tuple{post.type, post.author, post.likes}; }));
```

#### Group by on Single Column with modified value type
Let's group by stream of blogposts with respect to its types and return sets of the blog post.
```c++
  auto comparator = [](auto a1, auto a2) {
    return a1.likes < a2.likes;
  };

  auto groupedBy = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.type; },
                                                streams::Collectors::toSet<BlogPost, decltype(comparator)>(comparator)));

```

#### Cascading Group by
Group by all posts to there author, and then to the type of the blog 
```c++
map<string, map<BlogPostType, vector<BlogPost>>> groupedBy = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.author; },
                                                streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.type; })));

```

#### Aggregate results
- Get sum of all likes per post type
```c++
map<BlogPostType, long> groupedBy = dataset.stream().collect(
                                          streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.type; },
                                                                                    streams::Collectors::summingLong([](auto post) { return post.likes; })));

```
- Get average likes per post type
```c++
map<BlogPostType, double> groupedBy = dataset.stream().collect(
                                          streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.type; },
                                                                                    streams::Collectors::averaging([](auto post) { return post.likes; })));

```
- Get number of posts and there titles for each author
```c++
struct PostCountTitles {
  long postCount;
  std::string titles;
}; 
map<string, PostCountTitles> groupedBy = dataset.stream().collect(
                                               streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.author; },
                                                                                         streams::Collectors::collectingAndThen(streams::Collectors::toVector<BlogPost>(), [](auto posts) {
                                                                                           iterators::ListIterator iterator(posts.begin(), posts.end());
                                                                                           streams::Stream<BlogPost> postStream(iterator);
                                                                                           auto count = postStream.collect(streams::Collectors::counting());
                                                                                           auto titles = postStream.map([](auto post) { return post.title; })
                                                                                                                    .collect(streams::Collectors::joining(" : "));
                                                                                           return PostCountTitles{count, titles};
                                                                                         })));
```

### Collect as containers

#### Collect as vector

```c++
auto vector = data.stream().collect(streams::Collectors::toVector<int>());
```
#### Collect as set
```c++
auto set = data.stream().collect(streams::Collectors::toSet<int>());
```
#### Collect as any container
```c++
  std::unordered_set<int> unorderedSet;
  auto outSet = data.stream().collect(streams::Collectors::toContainer(unorderedSet));
  std::list<int> list;
  auto outList = data.stream().collect(streams::Collectors::toContainer(list));
  std::deque<int> dq;
  auto outDq = data.stream().collect(streams::Collectors::toContainer(dq));
```

#### Collect as map
```c++
struct AType {
  int a;
  char b;
  std::string c;
};

map<int, string> result1= dataset.stream().collect(streams::Collectors::toMap<AType>([](auto pod) { return pod.a; }, [](auto pod) { return pod.c; }));
  
map<char,string> result2 = dataset.stream().collect(streams::Collectors::toMap<AType>([](auto pod) { return pod.b; }, [](auto pod) { return pod.c; }, [](auto a1, auto a2) { return a1 + ", " + a2; }));
```

### Counting and Then
```c++
collection::Svector vector{12, 2, 13, 4, 5};
vector.stream().collect(streams::Collectors::collectingAndThen(streams::Collectors::counting(), [](auto count) { std::cout << count << std::endl; }));
```

### Joining string
```c++
collection::Svector vector{"apple", "boy", "cat", "dog", "elephant", "fish", "girl"};
string joined1 = vector.stream().collect(streams::Collectors::joining());
// apple boy cat dog elephant fish girl
string joined2 = vector.stream().collect(streams::Collectors::joining(","));
// apple,boy,cat,dog,elephant,fish,girl
string joined3 = vector.stream().collect(streams::Collectors::joining(", ", "[", "]"));
// [apple, boy, cat, dog, elephant, fish, girl]
```

### Partitioning by
```c++
collection::SVector data{21, 20, 29, 10, 17, 16, 40, 50};

map<bool, vector<int>> result1 = data.stream().collect(streams::Collectors::partitioningBy<int>([](auto element) { return element % 2 == 0; }));
EXPECT_EQ(2, result1.size());
EXPECT_THAT(result1[true], ::testing::ElementsAre(20, 10, 16, 40, 50));
EXPECT_THAT(result1[false], ::testing::ElementsAre(21, 29, 17));

map<bool, long> result2 = data.stream().collect(streams::Collectors::partitioningBy<int>([](auto element) { return element % 2 == 0; }, streams::Collectors::counting()));
EXPECT_EQ(2, result2.size());
EXPECT_EQ(result2[true], 5);
EXPECT_EQ(result2[false], 3);

```

### Averaging
```c++
collection::SVector data{12, 2, 13, 4, 5};
double avg = data.stream().collect(streams::Collectors::::averaging([](auto element) { return element * 1.0; });
// 7.2
```

### Counting
```c++
collection::SVector data{12, 2, 13, 4, 5};
size_t count = data.stream().collect(streams::Collectors::::counting());
// 5
```

### Summing
```c++
collection::SVector data{12, 2, 13, 4, 5};
long sum1 = data.stream().collect(streams::Collectors::::summingLong([](auto element) { return element; });
// 36
double sum2 = data.stream().collect(streams::Collectors::::summingLong([](auto element) { return element * 1.0; });
// 36.0
```

