cmake_minimum_required(VERSION 3.10)
project(PokemonFMG)

set(CMAKE_CXX_STANDARD 11)

# Set the build type if not explicitly specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

# Set Emscripten-specific settings only if building for Emscripten
if (EMSCRIPTEN)
    # Find the Emscripten SDK path from the environment variable
    if(NOT EMSCRIPTEN_ROOT_PATH)
        message(FATAL_ERROR "EMSCRIPTEN_ROOT_PATH not set. Please make sure Emscripten is set up correctly.")
    endif()

    # Include the emscripten.cmake file
    include(${EMSCRIPTEN_ROOT_PATH}/cmake/Modules/Platform/Emscripten.cmake)

    # Set Emscripten compiler flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s GL_ENABLE_GET_PROC_ADDRESS=1 -sFULL_ES2")
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
    preprocess_directory("assets/OW_SPRITE")
    preprocess_directory("assets/SPRITEFONT")
    preprocess_directory("assets/TILEMAPS")
    preprocess_directory("assets/CSV")
    preprocess_directory("assets/STAT_SPRITES")
    preprocess_directory("assets/MISC")
    preprocess_directory("assets/SFX")
    preprocess_directory("assets/VN_SPRITE")
    preprocess_directory("assets/PKMN_BACKSPRITE")
    preprocess_directory("assets/PKMN_BATTLESPRITE")
    preprocess_directory("assets/TRAINER_BACKSPRITE")
    preprocess_directory("assets/TRANSITIONS")

    # Concatenate all preload files into a single string
    string(REPLACE ";" " " PRELOAD_FILES "${PRELOAD_FILES}")

    # Add the preload files to CXX flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PRELOAD_FILES}")
endif()

# Windows-specific settings
if(WIN32)
    # Target Windows XP
    set(CMAKE_SYSTEM_VERSION 5.1)
    # Set compiler flags
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif()

# Set the output directory for the built executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")

# Add include and source directories
include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add executable
add_executable(PokemonFMG ${SOURCES} ${C_SOURCES})

# Set up FetchContent
include(FetchContent)

# Fetch raylib
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
)
FetchContent_MakeAvailable(raylib)

# Fetch LDtkLoader
FetchContent_Declare(
    LDtkLoader
    GIT_REPOSITORY https://github.com/Madour/LDtkLoader.git
    GIT_TAG master # You can specify a specific tag if needed
)
FetchContent_MakeAvailable(LDtkLoader)

# Make sure raylib is compiled as a static library
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # Disable building raylib examples

# Add external libraries (using only keyword-based signature)
target_link_libraries(PokemonFMG PRIVATE raylib LDtkLoader)
