
file(GLOB GRAPHICS_FILES "*.cpp" "*.h")

add_library(Graphics ${GRAPHICS_FILES})
set_property(TARGET Graphics PROPERTY FOLDER "2. Modules") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GRAPHICS_FILES})
