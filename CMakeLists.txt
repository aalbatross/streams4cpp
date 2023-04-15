cmake_minimum_required(VERSION 3.20)
project(streams4cpp CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories("/usr/local/include" ${CMAKE_CURRENT_SOURCE_DIR})
link_directories("/usr/local/lib")

add_library(streams4cpp INTERFACE iterator.h listiterator.h stream.h listiterator_view.h)

add_subdirectory(test)