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
