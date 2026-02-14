add_library(uthreads
        # public api
        include/uthreads/uthreads.hpp
        include/uthreads/Thread.hpp
        include/uthreads/Atomic.hpp
        # include/uthreads/Mutex.hpp

        # implementation
        src/Timer.hpp
        src/Thread.cpp
        src/Timer.cpp
        src/Scheduler.cpp
        src/Scheduler.hpp
        src/TCB.hpp
        src/TCB.cpp
        # src/Mutex.cpp
)

target_include_directories(uthreads
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)