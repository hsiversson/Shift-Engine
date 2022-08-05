file(GLOB_RECURSE APPLICATION_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_library(Application ${APPLICATION_FILES})
set_property(TARGET Application PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${APPLICATION_FILES})

target_include_directories(Application PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core)

target_precompile_headers(Application PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core/Common/SC_Base.h)