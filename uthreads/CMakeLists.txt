add_library(uthreads
        include/Thread.hpp
        src/Timer.hpp
        src/Thread.cpp
        src/Timer.cpp
        src/Scheduler.cpp
        src/Scheduler.hpp
        src/TCB.hpp
        src/TCB.cpp
)

target_include_directories(uthreads
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)