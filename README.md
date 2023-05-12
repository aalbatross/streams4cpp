## Introduction

streams4cpp is a header only C++ library which implements [MapReduce](https://en.wikipedia.org/wiki/MapReduce) programming primitive in C++ 17. It aims to provide C++ programmers
with map/reduce style stream based one-liner operations on bounded and unbounded container of elements.

Streams differ from collections in several ways:

- **No storage**:  A stream does not store elements; instead, it transforms element from backing source such as a data structure, an array, a generator function, or an I/O channel, through a series of computational operations.
- **Functional in nature**: Streaming operations are defined as functions, which produces a result, but does not modify its source.
- **Lazy evaluation**: Stream is evaluated in lazily on reduce or collect operation, all the other operations like map, filter, distinct, sorted which are non-terminal operations only updates the internal computation. 
- **Possibly unbounded**: While collections have a finite size, streams need not. Short-circuiting operations such as limit(n) or findFirst() can allow computations on infinite streams to complete in finite time.
- **Consumable**: The elements of a stream are consumable w.r.t to operations defined in the pipeline.

## Features

- **Stream**: Perform map/reduce operation on the bounded/ unbounded data containers or Iterators.
- **Streamable Collection**: Streamable STL containers supporting functional style transforms.
- **Collectors**: Implementations of Collector that implement various useful common reduction operations.

## Documentation
Refer [documentation](https://aalbatross.github.io/streams4cpp/doc/html/index.html) for detailed API review.

## Illustrative Examples
### Word Count in a File
Counting frequency of each word in the file.
```c++
std::ifstream file("story.txt");
std::istream_iterator<std::string> begin_it(file);
std::istream_iterator<std::string> end_it;

streams::UBStream<std::string> newStream(std::move(begin_it), std::move(end_it));
  auto wordCount = newStream
                      .collect(streams::Collectors::groupingBy<std::string>(
                        [] (auto element) {return element;},
                        streams::Collectors::counting()
                        ));

  for (const auto& word: wordCount) {
    std::cout << word.first << " -> " << word.second << std::endl;
  }
```
### Moving average on stream
Calculating moving average from prices in sliding window of 2.
```c++
std::vector prices{110.0, 213.90, 311.69, 412.23, 512.1, 610.03, 1000.0, 2102.12};
streams::UBStream<double> stream(data.begin(), data.end());
auto movingAverageStream = stream
                              .sliding(2)
                              .map([](auto window) { return window.stream().collect(Collectors::averaging()); });
auto movingAverage = movingAverageStream.toVector();
```

### Reading CSV file line by line to POD(Plain Old data)
```c++
struct Employee {
  long employeeId;
  std::string firstName;
  std::string lastName;
  std::string email;
  std::string phoneNo;
  std::string hireDate;
  std::string jobId;
  long salary;
  long commissionPct;
  long managerId;
  long departmentId;

  friend std::ostream &operator<<(std::ostream &os, const Employee &employee) {
    os << "employeeId: " << employee.employeeId << " firstName: " << employee.firstName << " lastName: " << employee.lastName << " email: " << employee.email << " phoneNo: " << employee.phoneNo << " hireDate: " << employee.hireDate << " jobId: " << employee.jobId << " salary: " << employee.salary << " commissionPct: " << employee.commissionPct << " managerId: " << employee.managerId << " departmentId: " << employee.departmentId;
    return os;
  }
};

class Line: public std::string {
  friend std::istream & operator>>(std::istream & is, Line & line)
  {
    return std::getline(is, line);
  }
};

std::ifstream file("/path/to/file/employees.csv");
std::istream_iterator<Line> begin_it(file);
std::istream_iterator<Line> end_it;

streams::UBStream<Line> newStream(std::move(begin_it), std::move(end_it));
// skip 1 skips the header
auto employees = newStream.skip(1).map([](auto line) {
                                      size_t pos = 0;
                                      std::string token;
                                      std::vector<std::string> words;
                                      std::string delimiter = ",";
                                      while ((pos = line.find(delimiter)) != std::string::npos) {
                                        token = line.substr(0, pos);
                                        words.emplace_back(token);
                                        line.erase(0, pos + delimiter.length());
                                      }

                                      return Employee{
                                          std::strtol(words[0].c_str(), nullptr, 10),
                                          words[1],
                                          words[2],
                                          words[3],
                                          words[4],
                                          words[5],
                                          words[6],
                                          std::strtol(words[7].c_str(), nullptr, 10),
                                          std::strtol(words[8].c_str(), nullptr, 10),
                                          std::strtol(words[9].c_str(), nullptr, 10),
                                          std::strtol(words[10].c_str(), nullptr, 10)};
                                    })
                                    //filters all the employees whose salary is greater than equals to 10000
                                    .filter([](const auto emp) {return emp.salary >= 10000;})
                                    .toVector();

for (const auto &employee : employees) {
  std::cout << employee << std::endl;
}
```

## Installation Guide
### Pre-requisites
- doxygen -1.9.6
- clang-format
- clang-tidy
- conan2
- cmake 3.25
- GCC/ any c++ compiler (latest version C++17 supported) like GCC-12.0, Apple-CLang 14.0 etc
- git

Run the following command to get the library installed:
```commandline
% git clone https://github.com/aalbatross/streams4cpp.git
% cmake -S. -Brelease -DCMAKE_BUILD_TYPE=Release
% cmake --build release/.
% ./release/test/streams4cpp.t 
%  cmake --install release/.
```




