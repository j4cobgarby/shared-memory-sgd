cmake_minimum_required(VERSION 3.5.1)
project(mininn)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
SET(CMAKE_CXX_FLAGS_DEBUG "-O2 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("/usr/local/include" "/usr/local/opt/llvm/include", "/usr/include/eigen3", "/usr/include/eigen3/unsupported")
link_directories("/usr/local/lib" "/usr/local/opt/llvm/lib")

set(APP_INCLUDES
        include/
        include/minidnn
 )

#Include the directories and now your cpp files will recognize your headers
include_directories(${APP_INCLUDES})

add_executable(mininn main.cpp elasync.cpp single_thread.cpp sync.cpp async.cpp leashed.cpp semisync.cpp heuristic.cpp elasync2.cpp)
target_sources(mininn PUBLIC mnist.cc)
TARGET_LINK_LIBRARIES(mininn
        pthread
        #"${TORCH_LIBRARIES}"
        )


