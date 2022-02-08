
file(GLOB MEMORY_FILES "*.cpp" "*.h")

add_library(Memory ${MEMORY_FILES})
set_property(TARGET Memory PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MEMORY_FILES})