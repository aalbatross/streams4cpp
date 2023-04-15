cmake_minimum_required(VERSION 3.20)
project(streams4cpp CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} "/usr/local/include")
link_directories("/usr/local/lib")

add_library(streams4cpp INTERFACE
        aalbatross/utils/iterator.h
        aalbatross/utils/listiterator.h
        aalbatross/utils/listiterator_view.h
        aalbatross/utils/stream.h
        )

target_include_directories(streams4cpp INTERFACE aalbatross/utils)

add_subdirectory(test)