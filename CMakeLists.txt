cmake_minimum_required(VERSION 3.10)
project(BenchmarkTest)

# Set C++14 standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include/utils)

# Add the library
add_library(benchmark_lib 
    src/utils/Benchmark.cpp
)

# Set include directories for the library
target_include_directories(benchmark_lib PUBLIC 
    ${CMAKE_SOURCE_DIR}/include/utils
)

# Add the test executable
add_executable(test_benchmark
    src/utils/test_benchmark.cpp
)

# Link the library
target_link_libraries(test_benchmark benchmark_lib)

# Enable testing
enable_testing()
add_test(NAME benchmark_test COMMAND test_benchmark)
