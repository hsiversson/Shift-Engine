
file(GLOB PHYSICS_FILES "*.cpp" "*.h")

add_library(Physics ${PHYSICS_FILES})
set_property(TARGET Physics PROPERTY FOLDER "2. Modules") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PHYSICS_FILES})
