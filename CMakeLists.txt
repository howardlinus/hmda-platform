cmake_minimum_required(VERSION 3.10)
project(FeatureBuilder VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Create a library from the source files
add_library(featurebuilder STATIC
    src/FeatureBuilder.cpp
)

target_include_directories(featurebuilder PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Build the example executable
add_executable(run_arimax_infer
    examples/run_arimax_infer.cpp
)

target_link_libraries(run_arimax_infer
    featurebuilder
)

# Installation rules (optional)
install(TARGETS featurebuilder run_arimax_infer
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
)
