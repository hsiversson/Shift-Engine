##################################
# Required CMake version
cmake_minimum_required(VERSION 3.18)

##################################
# Solution Name + Version
project(ShiftEngine VERSION 1.0)

##################################
# Set various C++ config variables
set(CMAKE_CXX_STANDARD 17)                  ## C++17 Standard
set(CMAKE_CXX_STANDARD_REQUIRED True)       ## Enforce the C++17 Standard

##################################
# Repository folder structure setup
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Bin)     ## Where to put the .exe files etc.
set(ENGINE_SOURCE_DIRECTORY ${PROJECT_SOURCE_DIR}/Engine/Source)  ## Where is the engine source code located
set(GAME_SOURCE_DIRECTORY ${PROJECT_SOURCE_DIR}/Game/Source)      ## Where is the game source code located

##################################
# Misc settings
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

##################################
# Global Defines
add_definitions(-DUNICODE -D_UNICODE)

##################################
# Compiler options
if (MSVC)
    add_compile_options(-W4 -WX)                            ## Adds warning level 4 and enables warnings as errors
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)    ## GCC/CLANG version of warning level 4 and warnings as errors
endif()


##################################
# Repository sub-directories
add_subdirectory(${ENGINE_SOURCE_DIRECTORY})
add_subdirectory(${GAME_SOURCE_DIRECTORY})