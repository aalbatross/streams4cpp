## Introduction

streams4cpp is a header only C++ library which implements jvm like streams in C++. It aims to provide C++ programmers
with map/reduce style stream based one-liner operations on bounded and unbounded container of elements.

Streams differ from collections in several ways:

- **No storage**:  A stream is not a data structure that stores elements; instead, it conveys elements from a source such as a data structure, an array, a generator function, or an I/O channel, through a pipeline of computational operations.
- **Functional in nature**: An operation on a stream produces a result, but does not modify its source. For example, filtering a Stream obtained from a collection produces a new Stream without the filtered elements, rather than removing elements from the source collection.
- **Laziness-seeking**: Many stream operations, such as filtering, mapping, or duplicate removal, can be implemented lazily, exposing opportunities for optimization. For example, "find the first String with three consecutive vowels" need not examine all the input strings. Stream operations are divided into intermediate (Stream-producing) operations and terminal (value- or side-effect-producing) operations. Intermediate operations are always lazy.
- **Possibly unbounded**: While collections have a finite size, streams need not. Short-circuiting operations such as limit(n) or findFirst() can allow computations on infinite streams to complete in finite time.
- **Consumable**: The elements of a stream are only visited once during the life of a stream. Like an Iterator, a new stream must be generated to revisit the same elements of the source.

## Features

- **Iterators**: Interfaces and classes defining bound or unbound traversal of data containers or any data source.
- **Stream**: Perform map/reduce operation on the bounded data containers or Iterators.
- **Streamable Collection**: Streamable STL containers supporting functional style transforms.
- **Collectors**: Implementations of Collector that implement various useful common reduction operations.

## Quick Start

An example of using Streams is provided below:

### Creating Stream

```
std::vector data{1, 2, 3, 4, 5};
ListIterator iter(data.begin(), data.end());
Stream<int> stream(iter);
```
or
```
collection::SVector svector{1, 2, 3, 4, 5};
auto stream = svector.stream();
```

### Mapping to double

```
const auto doubler = [](const auto element) { return element * 2.0; };
const auto plusHunderead = [](const auto element) { return element + 100; };
auto newStream = stream.map(doubler).map(plusHunderead);
```

[102.0, 104.0, 106.0, 108.0, 110.0]

### Filtering stream

```
const auto greaterThan4 = [](auto number) { return number > 105.0; };
auto filteredStream = newStream.filter(greaterThan4);
```

[106.0, 108.0, 110.0]

### Reducing stream

```
const auto sumAccumulator = [](auto x_1, auto y_1) { return x_1 + y_1; };
auto sum = newStream.reduce(0, sumAccumulator);
ASSERT_EQ(sum, 530);
```

### Printing Stream

```
const auto print = [](const auto &element) { std::cout << element << std::endl; };
const auto toString = [](const auto element) { return std::to_string(element) + " something"; };
filteredStream.map(toString).forEach(print);
```

106.0 something <br />
108.0 something <br />
110.0 something <br />

### Stream to containers

```
  std::vector data{1, 2, 3, 4, 5};
  ListIterator iter(data.begin(), data.end());
  Stream<int> stream(iter);
  auto filteredStream = stream.map(doubler).filter(greaterThan4);
  EXPECT_THAT(filteredStream.toVector(), ::testing::ElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toSet(), ::testing::UnorderedElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toUnorderedSet(), ::testing::UnorderedElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toList(), ::testing::ElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toDeque(), ::testing::ElementsAre(6, 8, 10));
  EXPECT_THAT(filteredStream.toVector(), ::testing::ElementsAre(6, 8, 10));
```

### Stream head,tail and find

```
  std::vector data{21, 20, 10, 16, 40, 50};
  ListIterator iter(data.begin(), data.end());
  Stream<int> stream(iter);
  EXPECT_EQ(stream.head().value(), 21);
  EXPECT_EQ(stream.tail().value(), 50);
  EXPECT_EQ(stream.find([](auto num) { return num % 25 == 0; }).value(), 50);
```

### Sorted Stream

```
  std::vector data{121, 12, 123, 41, 59};
  ListIterator iter(data.begin(), data.end());
  Stream<int> stream(iter);
  EXPECT_THAT(stream.sorted(std::greater<>()).toVector(), ::testing::ElementsAre(123, 121, 59, 41, 12));
  EXPECT_THAT(stream.sorted(std::less<>()).toVector(), ::testing::ElementsAre(12, 41, 59, 121, 123));
```

### Distinct element stream

```
  std::vector data{1, 2, 3, 4, 5, 2, 3, 4, 5};
  ListIterator iter(data.begin(), data.end());
  Stream<int> stream(iter);
  EXPECT_THAT(stream.distinct().toVector(), ::testing::ElementsAre(1, 2, 3, 4, 5));
```

### Match on stream

```
  std::vector data{21, 20, 10, 16, 40, 50};
  ListIterator iter(data.begin(), data.end());
  Stream<int> stream(iter);
  EXPECT_TRUE(stream.allMatch([](auto num) { return num >= 10; }));
  EXPECT_FALSE(stream.allMatch([](auto num) { return num >= 20; }));
  EXPECT_TRUE(stream.anyMatch([](auto num) { return num >= 20; }));
  EXPECT_FALSE(stream.anyMatch([](auto num) { return num <= 0; }));
  EXPECT_TRUE(stream.noneMatch([](auto num) { return num <= 0; }));
  EXPECT_FALSE(stream.noneMatch([](auto num) { return num >= 0; }));
```

### Reversed Stream

```
  std::vector data{21, 20, 10, 16, 40, 50};
  ListIterator iter(data.begin(), data.end());
  Stream<int> stream(iter);
  EXPECT_THAT(stream.reverse().toVector(), ::testing::ElementsAre(50, 40, 16, 10, 20, 21));
```

### Group by on Stream

```
  std::vector data{21, 20, 29, 10, 17, 16, 40, 50};
  ListIterator iter(data.begin(), data.end());
  Stream<int> stream(iter);
  auto result = stream.groupedBy([](auto number) { return number % 2 == 0 ? "even" : "odd"; });
  EXPECT_EQ(2, result.size());
  EXPECT_THAT(result["even"], ::testing::ElementsAre(20, 10, 16, 40, 50));
  EXPECT_THAT(result["odd"], ::testing::ElementsAre(21, 29, 17));
```

## Installation Guide

### Pre-requisites
- doxygen -1.9.6
- clang-format
- clang-tidy
- conan2
- cmake 3.25
- GCC/ any c++ compiler (latest version C++17 supported)

```commandline
% conan install . --output-folder=release --build=missing
% cd release
% cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
% cmake --build .
% ./test/streams4cpp.t 
%  cmake --install .
```







