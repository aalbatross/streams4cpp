cmake_minimum_required(VERSION 3.25)
project(streams4cpp CXX)
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include(cmake/CPM.cmake)

find_program(
        CLANG_TIDY_EXE
        NAMES clang-tidy
)
if (CLANG_TIDY_EXE)
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    set(DO_CLANG_TIDY
            "${CLANG_TIDY_EXE}"
            )
    set(CMAKE_CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
else ()
    message(STATUS "clang-tidy not found.")
endif ()

add_library(${PROJECT_NAME} INTERFACE
        aalbatross/utils/iterator.h
        aalbatross/utils/listiterator.h
        aalbatross/utils/listiterator_view.h
        aalbatross/utils/stream.h
        )

target_include_directories(${PROJECT_NAME} INTERFACE aalbatross/utils)
file(GLOB files "${CMAKE_SOURCE_DIR}/aalbatross/utils/*.h")
install(FILES ${files} DESTINATION include/aalbatross/utils/)
add_subdirectory(test)