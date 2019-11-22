cmake_minimum_required(VERSION 3.1)

# Regrouping all the third_party sources
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/include)
add_subdirectory(googletest)

# Google benchmark
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/benchmark/include)
add_subdirectory(benchmark)