
file(GLOB PLATFORM_FILES "*.cpp" "*.h")

add_library(Platform ${PLATFORM_FILES})
set_property(TARGET Platform PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PLATFORM_FILES})
