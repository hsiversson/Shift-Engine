
file(GLOB GAMEFRAMEWORK_FILES "*.cpp" "*.h")

add_library(GameFramework ${GAMEFRAMEWORK_FILES})
set_property(TARGET GameFramework PROPERTY FOLDER "2. Modules") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GAMEFRAMEWORK_FILES})
