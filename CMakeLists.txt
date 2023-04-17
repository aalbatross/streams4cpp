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

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

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