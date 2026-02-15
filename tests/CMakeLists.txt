set(TEST_TARGETS
        simple_tests
        scheduler_tests
        atomic_tests
        mutex_tests
        multiprocessor_tests
)

add_executable(simple_tests SimpleTests.cpp)
add_executable(scheduler_tests SchedulerTests.cpp)
add_executable(atomic_tests AtomicTests.cpp)
add_executable(mutex_tests MutexTests.cpp)
add_executable(multiprocessor_tests MultiprocessorTests.cpp)

foreach(target IN LISTS TEST_TARGETS)
    target_link_libraries(${target} PRIVATE
            GTest::gtest
            GTest::gtest_main
            uthreads
    )
endforeach()

target_include_directories(scheduler_tests PRIVATE
        ${PROJECT_SOURCE_DIR}/uthreads/src
)

target_include_directories(multiprocessor_tests PRIVATE
        ${PROJECT_SOURCE_DIR}/uthreads/src
)

add_test(NAME SimpleTest COMMAND simple_tests)
add_test(NAME SchedulerTests COMMAND scheduler_tests)
add_test(NAME AtomicTests COMMAND atomic_tests)
add_test(NAME MutexTests.cpp COMMAND mutex_tests)
add_test(NAME MultiprocessorTests.cpp COMMAND multiprocessor_tests)
