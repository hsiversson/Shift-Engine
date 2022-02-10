file(GLOB_RECURSE GAMEFRAMEWORK_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_library(GameFramework ${GAMEFRAMEWORK_FILES})
set_property(TARGET GameFramework PROPERTY FOLDER "2. Modules") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GAMEFRAMEWORK_FILES})

target_include_directories(GameFramework PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core)

target_precompile_headers(GameFramework PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core/Common/SC_Base.h)