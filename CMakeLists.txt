cmake_minimum_required(VERSION 3.22)
project(scheduler)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.74.0 REQUIRED COMPONENTS program_options thread)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "13.1")
        message(FATAL_ERROR "Insufficient gcc version")
    endif()
endif()

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(scheduler src/scheduler.cpp
            src/scheduler.hpp
            src/main.cpp
            src/dag.hpp
            src/parser.hpp
    )
    target_link_libraries(scheduler ${Boost_LIBRARIES})
endif()