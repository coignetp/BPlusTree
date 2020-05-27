![CI](https://github.com/coignetp/BPlusTree/workflows/C/C++%20CI/badge.svg?branch=master)
# Description
This project is an implementation of a [BPlusTree](https://en.wikipedia.org/wiki/B%2B_tree) in C++. The goal is to make an easy to use performant single header library.

The main goal is for me to be confronted to performances issues in C++ and try to improve it.

This project uses gtest for the tests, google benchmark for benchmarks. They are included as submodules in this repository.
# Installation
## Prerequisites
C++ version must be at least C++14.  
CMake must be at least version 3.1.

## Compilation
```
mkdir build
cd build
cmake ..
cmake --build .
```
There are 2 output executable, one for the test and one for the benchmark.

## External use
You can use this BPlusTree with simply including the header file `src/bptree.hpp`. A documentation will be made (TODO).

Example in code:
```C++
#include "bptree.hpp"

#include <iostream>

// This hash function should be defined by the user
uint64_t hashFunc(int a) {
  return a;
}

int main(int argc, char **argv) {
  // Degree of the BPlusTree and the hash function of this type
  BPT::BPTree<int> bpt(5, hashFunc);

  // Add an item to the tree
  bpt.AddItem(4);

  // Search an item in the tree
  if (bpt.SearchItem(4) == 4)
    std::cout << "Item found" << std::endl;

  // Delete an item in the tree
  bpt.DeleteItem(4);

  return 0;
}
```
