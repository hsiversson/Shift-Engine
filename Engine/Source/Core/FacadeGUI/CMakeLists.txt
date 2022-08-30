file(GLOB_RECURSE FACADE_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_library(Facade ${FACADE_FILES})
set_property(TARGET Facade PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FACADE_FILES})

##################################
# Target Settings
target_include_directories(Facade PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core)

##################################
# Precompiled Header
target_precompile_headers(Facade PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core/Common/SC_Base.h)