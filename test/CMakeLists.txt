set(NAME streams4cpp.t)

add_executable(${NAME} test_listiterator.t.cpp test_listiterator_view.t.cpp test_stream.t.cpp)

CPMAddPackage("gh:/google/googletest@1.13.0")
target_link_libraries(${NAME} PUBLIC streams4cpp gmock gtest gtest_main)

enable_testing()

add_test(
        NAME streams4cpp.t
        COMMAND streams4cpp.t
)