## Introduction

streams4cpp is a header only C++ library which implements jvm like streams in C++. It aims to provide C++ programmers
with map/reduce style stream based one-liner operations on bounded and unbounded container of elements.

Streams differ from collections in several ways:

- **No storage**:  AType stream does not store elements; instead, it transforms element from backing source such as a data structure, an array, a generator function, or an I/O channel, through a series of computational operations.
- **Functional in nature**: Streaming operations are defined as functions, which produces a result, but does not modify its source.
- **Lazy evaluation**: Stream is evaluated in lazily on reduce or collect operation, all the other operations like map, filter, distinct, sorted which are non-terminal operations only updates the internal computation. 
- **Possibly unbounded**: While collections have a finite size, streams need not. Short-circuiting operations such as limit(n) or findFirst() can allow computations on infinite streams to complete in finite time.
- **Consumable**: The elements of a stream are consumable w.r.t to operations defined in the pipeline.

## Features

- **Stream**: Perform map/reduce operation on the bounded data containers or Iterators.
- **Streamable Collection**: Streamable STL containers supporting functional style transforms.
- **Collectors**: Implementations of Collector that implement various useful common reduction operations.

## Documentation
Refer [documentation](https://aalbatross.github.io/streams4cpp/doc/html/index.html) for detailed API review.

## Installation Guide
### Pre-requisites
- doxygen -1.9.6
- clang-format
- clang-tidy
- conan2
- cmake 3.25
- GCC/ any c++ compiler (latest version C++17 supported) like GCC-12.0, Apple-CLang 14.0 etc
- git

```commandline
% git clone https://github.com/aalbatross/streams4cpp.git
% cmake -S. -Brelease -DCMAKE_BUILD_TYPE=Release
% cmake --build release/.
% ./release/test/streams4cpp.t 
%  cmake --install release/.
```




