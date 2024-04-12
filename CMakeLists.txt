cmake_minimum_required(VERSION 3.10)
project(PokemonFMG)

set(CMAKE_CXX_STANDARD 11)

# Set the build type if not explicitly specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

# Set Emscripten-specific settings only if building for Emscripten
if (EMSCRIPTEN)
    # Include the emscripten.cmake file
    include(/home/gatevs/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake)

    # Set Emscripten compiler flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s GL_ENABLE_GET_PROC_ADDRESS=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s GL_ENABLE_GET_PROC_ADDRESS=1")
    set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.

# Function to preprocess files in a directory and its subdirectories
function(preprocess_directory directory)
    file(GLOB_RECURSE files "${directory}/*")
    set(local_preload_files)
    foreach(file ${files})
        get_filename_component(file_name ${file} NAME)
        list(APPEND local_preload_files "--preload-file ${file}@/${directory}/${file_name}")
    endforeach()
    # Return the list of preloaded files
    set(PRELOAD_FILES ${PRELOAD_FILES} ${local_preload_files} PARENT_SCOPE)
endfunction()

# Include assets from different directories
preprocess_directory("assets")
preprocess_directory("assets/STAT_SPRITES")
preprocess_directory("assets/MISC")
preprocess_directory("assets/SFX")
preprocess_directory("assets/VN_SPRITE")

# Concatenate all preload files into a single string
string(REPLACE ";" " " PRELOAD_FILES "${PRELOAD_FILES}")

# Add the preload files to CXX flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PRELOAD_FILES}")
endif()

if(WIN32)
    # Target Windows XP
    set(CMAKE_SYSTEM_VERSION 5.1)
    # Set compiler flags
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
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
