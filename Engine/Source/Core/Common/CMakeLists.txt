
file(GLOB_RECURSE COMMON_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_library(Common ${COMMON_FILES})
set_property(TARGET Common PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${COMMON_FILES})

##################################
# Target Settings
target_include_directories(Common PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core)

##################################
# Precompiled Header
target_precompile_headers(Common PRIVATE SC_Base.h)