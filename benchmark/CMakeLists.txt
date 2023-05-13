set(NAME streams4cpp.b)

add_executable(${NAME}
    benchmark_collectors.b.cpp)

include(${CMAKE_BINARY_DIR}/benchmark-config.cmake)
include_directories(${benchmark_INCLUDE_DIRS})
target_link_libraries(${NAME} PUBLIC streams4cpp benchmark::benchmark benchmark::benchmark_main)