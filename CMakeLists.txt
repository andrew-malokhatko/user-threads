cmake_minimum_required(VERSION 4.1)
project(user_threads)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS -masm=intel)

add_subdirectory(uthreads)

add_executable(user_threads main.cpp)
target_link_libraries(user_threads PRIVATE uthreads)

target_include_directories(user_threads
    PUBLIC uthreads/include
)