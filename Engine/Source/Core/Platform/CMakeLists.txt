file(GLOB_RECURSE PLATFORM_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_library(Platform ${PLATFORM_FILES})
set_property(TARGET Platform PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PLATFORM_FILES})

##################################
# Target Settings
target_include_directories(Platform PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core/Platform)

##################################
# Precompiled Header
target_precompile_headers(Platform PRIVATE Platform/SC_Platform.h Atomics/SC_Atomics.h Debugging/SC_Assert.h)