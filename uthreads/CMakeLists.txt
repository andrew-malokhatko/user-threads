add_library(uthreads
    include/Thread.hpp
    include/Timer.hpp
    src/Thread.cpp
    src/Timer.cpp
)

target_include_directories(uthreads
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)