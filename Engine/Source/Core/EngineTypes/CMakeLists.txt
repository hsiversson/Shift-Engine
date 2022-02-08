
file(GLOB ENGINETYPES_FILES "*.cpp" "*.h")

add_library(EngineTypes ${ENGINETYPES_FILES})
set_property(TARGET EngineTypes PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINETYPES_FILES})