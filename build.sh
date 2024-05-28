#!/usr/bin/bash

mkdir -p cmake-build-debug
mkdir -p cmake-build

cd cmake-build-debug/
cmake  -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ../
cd ../cmake-build/
cmake  -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ../
