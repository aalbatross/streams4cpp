cmake_minimum_required(VERSION 3.25)
project(streams4cpp CXX)
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include(cmake/CPM.cmake)

add_library(${PROJECT_NAME} INTERFACE
        aalbatross/utils/iterator.h
        aalbatross/utils/listiterator.h
        aalbatross/utils/listiterator_view.h
        aalbatross/utils/stream.h
        )

target_include_directories(${PROJECT_NAME} INTERFACE aalbatross/utils)

add_subdirectory(test)