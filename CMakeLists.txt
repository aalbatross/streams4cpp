cmake_minimum_required(VERSION 3.25)
project(streams4cpp CXX)
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE ${CMAKE_CURRENT_SOURCE_DIR}
                    BUILD missing
                    OUTPUT_FOLDER ${CMAKE_BINARY_DIR}
                    REMOTE conancenter
                    SETTINGS ${settings})


file(GLOB_RECURSE ALL_SOURCE_FILES ${PROJECT_SOURCE_DIR}/aalbatross/utils/*.h)
file(GLOB ALL_TEST_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)
set(ALL_SOURCES ${ALL_SOURCE_FILES} ${ALL_TEST_FILES})


find_program(
        CLANG_FORMAT_EXE
        NAMES clang-format
)
if (CLANG_FORMAT_EXE)
    message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}")
    add_custom_target(
        clangformat ALL
        COMMAND clang-format
        --style=file
        --Werror
        --dry-run
        --verbose
        ${ALL_SOURCES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running CLang Format"
    )
else ()
    message(STATUS "clang-format not found.")
endif ()

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

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input file
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/doc/config/Doxyfile)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

add_library(${PROJECT_NAME} INTERFACE
        aalbatross/utils/iterators/iterator.h
        aalbatross/utils/iterators/listiterator.h
        aalbatross/utils/iterators/listiterator_view.h
        aalbatross/utils/streams/stream.h
        aalbatross/utils/utils.h
        aalbatross/utils/collection/streamablevector.h
        aalbatross/utils/collection/streamablecollection.h
        aalbatross/utils/collection/streamableset.h
        aalbatross/utils/collection/streamablemap.h
        aalbatross/utils/collection/streamablelist.h
        aalbatross/utils/collection/streamabledeque.h
        aalbatross/utils/collection/streamableunorderedset.h
        aalbatross/utils/collection/streamableunorderedmap.h
        aalbatross/utils/streams/collectors.h
        aalbatross/utils/streams/collector.h
        aalbatross/utils/streams/processor.h
        aalbatross/utils/streams/ub_stream.h )

target_include_directories(${PROJECT_NAME} INTERFACE aalbatross/utils)

install(DIRECTORY aalbatross DESTINATION include/)
add_subdirectory(test)
add_subdirectory(benchmark)