
file(GLOB_RECURSE ENGINETYPES_FILES CONFIGURE_DEPENDS "*.cpp" "*.h" "*.natvis")


add_library(EngineTypes ${ENGINETYPES_FILES})
set_property(TARGET EngineTypes PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINETYPES_FILES})

##################################
# Target Settings
target_include_directories(EngineTypes PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core)

##################################
# Precompiled Header
target_precompile_headers(EngineTypes PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core/Platform/Platform/SC_Platform.h ${ENGINE_SOURCE_DIRECTORY}/Core/EngineTypes/Math/SC_Math.h)