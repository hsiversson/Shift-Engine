
file(GLOB_RECURSE MEMORY_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_library(Memory ${MEMORY_FILES})
set_property(TARGET Memory PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MEMORY_FILES})

##################################
# Target Settings
target_include_directories(Memory PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core/Memory ${ENGINE_SOURCE_DIRECTORY}/Core/EngineTypes ${ENGINE_SOURCE_DIRECTORY}/Core/Platform)
target_link_libraries(Memory EngineTypes Platform)

##################################
# Precompiled Header
target_precompile_headers(Memory PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core/Platform/Platform/SC_Platform.h)