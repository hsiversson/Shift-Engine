
file(GLOB COMMON_FILES "*.cpp" "*.h")

add_library(Common ${COMMON_FILES})
set_property(TARGET Common PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${COMMON_FILES})