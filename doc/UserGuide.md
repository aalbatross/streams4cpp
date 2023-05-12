# User Guide
## Introduction
stream4cpp library facilitates 2 types of processing. 
1. **Batch Processing**: Batch Processing runs the computation/transformation/reduction/operation on the entire dataset one by one in parallel. This is memory intensive processing and keeps the copy of data and its intermediate states in memory for processing.
This type of streams can be useful when we want to do reductions/aggregation on the entire dataset where dataset size fits in system memory. Use _streams::Stream_ class to perform Batch processing.
2. **Stream Processing**:
Stream Processing runs the computation/transformation/reduction/operation one by one on each row/event of the dataset. This type of Stream is useful when we want to work on continuos stream of infinite data where aggregation on all data is difficult or impossible, However working on small windows of data for reduction and aggregations will be useful. Use _streams::UBStream_ class to perform Stream processing.

stream4cpp incorporates [lazy evaluation](https://en.wikipedia.org/wiki/Lazy_evaluation) to the entire stream, such that evaluation takes place only on terminal operations.

Each of the above implementations provide some common set of operations categorised as:
1. **Transformation**: These operation transforms the incoming data. This step where the source input can be enriched with variety of common transforms like, map, flatmap, filter, skip, limit, sorted, distinct, sliding, fixed etc.
2. **Reduction**: These operation reduces data to results, they are also terminal operations which concludes processing by yielding aggregate of data, some common operations are toVector, groupingBy, reduce, collect, joining, partitioningBy etc.

## Transformations
Transformation are the operations which transform the incoming data.
### Map
Map is a transform which works on copy of data to transform it into another type/ or structure. For example:

```c++
const auto doubler = [](const auto element) { return element * 2.0; };
const auto plusHunderead = [](const auto element) { return element + 100; };
const auto addTimes = [](const auto element) { return std::to_string(element) + " times"; };
auto output = stream.map(doubler).map(plusHunderead).map(addTimes).toVector();

// input: 1,2,3,4,5
// output: 102 times, 104 times, 106 times ...
```
Consider the above example, here the input stream is first mapped to double its value then to the doubled value add hunderead, and then to that value add times as suffix.
In this entire flow the input element is first converted to double, than add hunderead operation is performed and then the results is converted to string.

### Flatten
Flatten function flattens the incoming iterator and applies the provided map function to the flattened iterator. For example:

```c++
auto output = stream.flatten([](auto element){return element;}).toVector();

//input: {1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {20, 30, 40, 50}
//output: 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50
```
Consider the above example here input stream has vector of vectors, then the flatten function above returns the vector with all ints.

### Filter
Filter function filter all the elements from input stream which follows the predicate. For example:

```c++
auto output = stream.filter([](auto element){return element >= 100;}).toVector();
// input: 70, 80, 90, 100, 110, 120
// output: 100, 110, 120
```
Consider the above example here, where the input stream contains series of ints, and the operation above filters out all elements which is greater than equal to 100 as output.

### Skip
Skip function skips provided number of element in the stream, and sends the remaining elements in the stream. For example:

```c++
auto output = stream.skip(4).toVector();
//input: 1, 2, 3, 4, 5, 6, 7, 8
//output: 5, 6, 7, 8
```
Consider the above example here, where the input stream contains series of ints, and the operation above skips the first 4 elements and returns remaining as output.

### Limit
Limit function limits provided number of element in the stream, and skips all the remaining elements from the stream. For example:

```c++
auto output = stream.limit(4).toVector();
//input: 1, 2, 3, 4, 5, 6, 7, 8
//output: 1, 2, 3, 4
```
Consider the above example here, where the input stream contains series of ints, and the operation above prints the first 4 elements from the input stream as output.

### Distinct
Distinct function transform the input stream to a stream with non-repeating input elements in an ordered way, For example:
```c++
auto output = stream.distinct().toVector();
//input: 1, 2, 3, 3, 4, 4, 5, 4, 4, 5
//output: 1, 2, 3, 4, 5
```

### Sorted
Sorted function transform the input stream to a stream with sorted order of input elements, For example:
```c++
auto greaterOutput = stream.sorted(std::greater<>()).toVector();
auto lesserOutput = stream.sorted(std::less<>()).toVector();
//input: 121, 12, 123, 41, 59
//greaterOutput: 123, 121, 59, 41, 12;
//lesserOutput: 12, 41, 59, 121, 123
```

### Reverse
Reverse function transform the input stream to a stream in a opposite direction, For example:
```c++
auto output = stream.reverse().toVector();
//input: 1, 2, 3, 4, 5 
//output: 5, 4, 3, 2, 1
```

### Sliding Window
 Sliding window function creates a sliding window of defined size on incoming stream. For example:

```c++
auto output = stream.sliding(4).toVector();
//input: 1, 2, 3, 4, 5, 6, 7, 8 ....
//output: [1, 2, 3, 4], [2 ,3, 4, 5], [3, 4, 5, 6], [4, 5, 6, 7], [5, 6, 7, 8]
```

Consider the above example here, where the input stream contains series of ints, and the operation above create sliding window of defined elements from the incoming stream as output.

### Fixed Window
Fixed window function creates a non overlapping window of defined size on incoming stream. For example:

```c++
auto output = stream.fixed(4).toVector();
//input: 1, 2, 3, 4, 5, 6, 7, 8 ....
//output: [1, 2, 3, 4], [5 ,6, 7, 8] ...
```

Consider the above example here, where the input stream contains series of ints, and the operation above create fixed non overlapping window of defined size from the incoming stream as output.

## Reductions
These operations reduce the stream of data to results. These are terminal operations. 

### Reduce
Reduce operation takes 2 inputs one seed value of result and other as binary operation on input to yield reduction on result. For example:
```c++
const auto sumAccumulator = [](auto x_1, auto y_1) { return x_1 + y_1; };
auto sum = newStream.reduce(0, sumAccumulator);

//input: 1, 2, 3, 4, 5
//sum: 15
```
Example above shows sum of input stream as reduce operation. Here the 0 in the reduce is seed value, and sumAccumulator is binary operation which is applied on input data from left to right to yield sum of the stream. 

### For Each
For Each Operation is applied on the input stream of elements, and the function is provided with consumer which accept the element from the input stream. For example:

```c++
const auto print = [](const auto &element) { std::cout << element << std::endl; };
const auto doubler = [](const auto element) { return element * 2.0; };
const auto plusHunderead = [](const auto element) { return element + 100; };
const auto addTimes = [](const auto element) { return std::to_string(element) + " times"; };
stream.map(doubler).map(plusHunderead).map(addTimes).forEach(print);
//input: 1,2,3,4,5
//output:
102 times
104 times
106 times
108 times
110 times
```
In the above example we demonstrate an example of printing underlying stream of data.

### Match
Match function checks the entire stream for a given predicate and return true/false if match predicate succeeds. Examples of different type of matches are given below:

```c++
EXPECT_TRUE(stream.allMatch([](auto num) { return num >= 10; }));
EXPECT_FALSE(stream.allMatch([](auto num) { return num >= 20; }));
EXPECT_TRUE(stream.anyMatch([](auto num) { return num >= 20; }));
EXPECT_FALSE(stream.anyMatch([](auto num) { return num <= 0; }));
EXPECT_TRUE(stream.noneMatch([](auto num) { return num <= 0; }));
EXPECT_FALSE(stream.noneMatch([](auto num) { return num >= 0; }));
//input: 21, 20, 10, 16, 40, 50
```

### Head, Tail and Find
Head, Tail and Find function provides first, last and finds an element in the input stream respectively. Example of Head, tail and find is as follows:

```c++
EXPECT_EQ(stream.head().value(), 21);
EXPECT_EQ(stream.tail().value(), 50);
EXPECT_EQ(stream.find([](auto num) { return num % 25 == 0; }).value(), 50);
//input: 21, 20, 10, 16, 40, 50
```
## Collectors
Collectors are special set of reducers which provide some common pattern of reductions on stream.
This section we provide examples of Collectors on Blogpost type.
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

### Group by
#### Group by on Single Column
Get all blog posts grouped by post type
```c++
map<BlogPostType, vector<BlogPost>> groupedBy = dataset.stream().collect(streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.type; }));
```

#### Group by on Single Column with modified key type
- Get all blog posts grouped by blog author and blog post type.
```c++
map<pair<BlogPostType, string>, vector<BlogPost>> groupedBy = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return std::pair{post.type, post.author}; }));
```
- Get all blog posts grouped by blog author ,blog post type and number of likes.
```c++
map<tuple<BlogPostType, string, int>, vector<BlogPost>> groupedBy = dataset.stream().collect(
      streams::Collectors::groupingBy<BlogPost>([](auto post) { return std::tuple{post.type, post.author, post.likes}; }));
```

#### Group by on Single Column with modified value type
- Get set of blog posts grouped by post type
```c++
auto comparator = [](auto a1, auto a2) {
  return a1.likes < a2.likes;
};
auto groupedBy = dataset.stream().collect(
    streams::Collectors::groupingBy<BlogPost>([](auto post) { return post.type; },
                                              streams::Collectors::toSet<BlogPost, decltype(comparator)>(comparator)));
```

#### Cascading Group by
Get all blog posts group by author and then blogpost type
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

