add_executable(simple_tests
        SimpleTests.cpp
)

target_link_libraries(simple_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
        uthreads
)

add_test(NAME SimpleTest COMMAND simple_tests)
