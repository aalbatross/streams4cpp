set(NAME streams4cpp.t)

add_executable(${NAME}
        test_listiterator.t.cpp
        test_listiterator_view.t.cpp
        test_stream.t.cpp
        test_scollection.t.cpp
        test_collector.t.cpp
        test_streamcollector.t.cpp
        test_ub_stream.t.cpp)

find_package(GTest CONFIG REQUIRED)
include_directories(${GTest_INCLUDE_DIR})
target_link_libraries(${NAME} PUBLIC streams4cpp GTest::gmock GTest::gtest GTest::gtest_main)

enable_testing()

add_test(
        NAME streams4cpp.t
        COMMAND streams4cpp.t
)