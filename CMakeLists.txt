cmake_minimum_required(VERSION 3.27)
project(scheduler)

set(CMAKE_CXX_STANDARD 20)
set(Boost_DEBUG ON)
find_package(Boost COMPONENTS ALL)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.83.0 REQUIRED COMPONENTS program_options)

if(Boost_FOUND)
    message("Boost found")
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(scheduler src/scheduler.cpp
            src/scheduler.hpp
            src/main.cpp
    )
    target_link_libraries(scheduler ${Boost_LIBRARIES})
endif()