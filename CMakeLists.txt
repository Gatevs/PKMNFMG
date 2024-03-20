cmake_minimum_required(VERSION 3.10)
project(PokemonFMG)

set(CMAKE_CXX_STANDARD 11)

# Set the build type if not explicitly specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

# Set the output directory for the built executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")

# Add include directories
include_directories(include)

# Add your source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Gather .c source files
file(GLOB_RECURSE C_SOURCES "src/*.c")

# Add executable
add_executable(PokemonFMG ${SOURCES} ${C_SOURCES})

# Add Tileson library
add_subdirectory(raylib)

# Add Tileson library
add_subdirectory(LDtkLoader)

# Add external libraries (using only keyword-based signature)
target_link_libraries(PokemonFMG PRIVATE raylib LDtkLoader)
