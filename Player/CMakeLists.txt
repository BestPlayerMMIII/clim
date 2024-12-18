cmake_minimum_required(VERSION 3.10)
project(Player)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include and sources directories
include_directories(include)
file(GLOB SOURCES src/*.cpp)

# Set build output directory to project Player directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Create the executable
add_executable(player ${SOURCES})

# Compiler options
if(MSVC)
    target_compile_options(player PRIVATE /W4 /O2)
else()
    target_compile_options(player PRIVATE -Wall -Wextra -O2)
endif()
