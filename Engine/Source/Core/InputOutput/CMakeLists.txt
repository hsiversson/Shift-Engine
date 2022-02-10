file(GLOB_RECURSE IO_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_library(InputOutput ${IO_FILES})
set_property(TARGET InputOutput PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${IO_FILES})

##################################
# Target Settings
target_include_directories(InputOutput PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core/InputOutput ${ENGINE_SOURCE_DIRECTORY}/Core/Memory ${ENGINE_SOURCE_DIRECTORY}/Core/EngineTypes ${ENGINE_SOURCE_DIRECTORY}/Core/Platform)
target_link_libraries(InputOutput Memory EngineTypes Platform)

##################################
# Precompiled Header
target_precompile_headers(InputOutput PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core/Platform/Platform/SC_Platform.h ${ENGINE_SOURCE_DIRECTORY}/Core/EngineTypes/SC_TypesInclude.h)