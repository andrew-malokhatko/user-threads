cmake_minimum_required(VERSION 4.1)
project(user_threads)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS -masm=intel)

add_executable(user_threads main.cpp
        include/Thread.hpp
        src/Thread.cpp
        include/Timer.hpp
        src/Timer.cpp)
