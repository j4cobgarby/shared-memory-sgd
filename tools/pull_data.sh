#!/usr/bin/bash

mkdir cif100_tmp
cd cif100_tmp
wget https://www.cs.toronto.edu/~kriz/cifar-100-binary.tar.gz
tar xf cifar-100-binary.tar.gz
mkdir -p ../data/cifar-100
echo cifar-100-binary/*
cp cifar-100-binary/* ../data/cifar-100/
cd ..
rm -rf cif100_tmp
