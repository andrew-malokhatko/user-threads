cmake_minimum_required(VERSION 4.1)
project(user_threads)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS -masm=intel)

# Check system requirements
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(WARNING "This project targets Linux only. Current system: ${CMAKE_SYSTEM_NAME}. Build may fail.")
endif()

add_subdirectory(uthreads)

# optionally build tests
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()

    find_package(GTest REQUIRED)

    add_subdirectory(tests)
endif()

add_executable(user_threads main.cpp)
target_link_libraries(user_threads PRIVATE uthreads)

target_include_directories(user_threads
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/uthreads/include
)