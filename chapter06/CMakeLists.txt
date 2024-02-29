cmake_minimum_required(VERSION 3.14)
project(Benchmark_cache)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Ensure no optimizations and enable debugging info
set(CMAKE_BUILD_TYPE Debug)

# Find the Google Benchmark package
find_package(benchmark REQUIRED)

# Define your executable
add_executable(${PROJECT_NAME} main.cpp )

# Link Google Benchmark to the executable
target_link_libraries(${PROJECT_NAME} benchmark::benchmark)

set(CMAKE_BUILD_TYPE Debug)
