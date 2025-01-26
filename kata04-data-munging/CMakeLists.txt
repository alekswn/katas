cmake_minimum_required(VERSION 3.10)

# Set the project name
project(kata04-data-munging)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# Add the executable for part1
add_executable(part1 src/part1/part1.cpp)
