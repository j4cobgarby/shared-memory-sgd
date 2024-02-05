#!/usr/bin/bash
cd cmake-build-debug/
cmake -DCMAKE_PREFIX_PATH="$PWD"/../packages/libtorch -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ../
cd ../cmake-build/
cmake -DCMAKE_PREFIX_PATH="$PWD"/../packages/libtorch -DCMAKE_BUILD_TYPE=Release-G "Unix Makefiles" ../
