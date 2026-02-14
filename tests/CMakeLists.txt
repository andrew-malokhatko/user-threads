add_executable(simple_tests
        SimpleTests.cpp
)

add_executable(scheduler_tests
        SchedulerTests.cpp
)

add_executable(atomic_tests
        AtomicTests.cpp
)

add_executable(mutex_tests
        MutexTests.cpp
)

target_link_libraries(simple_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
        uthreads
)

target_link_libraries(scheduler_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
        uthreads
)

target_link_libraries(atomic_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
        uthreads
)

target_link_libraries(mutex_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
        uthreads
)

target_include_directories(scheduler_tests PRIVATE
        ${PROJECT_SOURCE_DIR}/uthreads/src
)

add_test(NAME SimpleTest COMMAND simple_tests)
add_test(NAME SchedulerTests COMMAND scheduler_tests)
add_test(NAME AtomicTests COMMAND atomic_test)
add_test(NAME MutexTests.cpp COMMAND mutex_tests)
