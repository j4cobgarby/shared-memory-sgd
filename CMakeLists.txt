cmake_minimum_required(VERSION 3.5.1)
project(mininn)

set(CMAKE_VERBOSE_MAKEFILE off)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -O1")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("/usr/local/include" "/usr/local/opt/llvm/include"
        "/usr/include/eigen3" "/usr/include/eigen3/unsupported"
        "include/" "include/minidnn" )
link_directories("/usr/local/lib" "/usr/local/opt/llvm/lib")

add_executable(mininn main.cpp elasync.cpp single_thread.cpp sync.cpp async.cpp leashed.cpp semisync.cpp heuristic.cpp
        elasync2.cpp mnist.cc)
#target_sources(mininn PUBLIC mnist.cc)
TARGET_LINK_LIBRARIES(mininn pthread)
