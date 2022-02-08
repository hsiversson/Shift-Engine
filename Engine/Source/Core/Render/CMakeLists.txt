
file(GLOB RENDER_FILES "*.cpp" "*.h")

add_library(Render ${RENDER_FILES})
set_property(TARGET Render PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RENDER_FILES})