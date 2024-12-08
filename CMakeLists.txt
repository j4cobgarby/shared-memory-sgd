cmake_minimum_required(VERSION 3.5.1)
project(mininn)

set(CMAKE_VERBOSE_MAKEFILE off)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -DEIGEN_DONT_PARALLELIZE")
# set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("/usr/local/include" "/usr/local/opt/llvm/include"
        "/usr/include/eigen3" "/usr/include/eigen3/unsupported"
        "include/" "include/minidnn" )
link_directories("/usr/local/lib" "/usr/local/opt/llvm/lib")

add_executable(mininn

    main.cpp
    mnist.cc

    # Component implementations
    Component/asyncdispatcher.cpp
    Component/semisyncdispatcher.cpp
    Component/model_interface.cpp
    Component/search_controller.cpp
    Component/sgdthread.cpp
    Component/simplebatchcontroller.cpp
    Component/threadworkerpool.cpp
    Component/windowmonitor.cpp
    Component/emamonitor.cpp
    Component/evalmonitor.cpp
    Component/systemexecutor.cpp
    Component/window_controller.cpp
    Component/patterncontroller.cpp
        Component/elasticbatchcontroller.cpp
)

TARGET_LINK_LIBRARIES(mininn pthread)
