
file(GLOB SCRIPTING_FILES "*.cpp" "*.h")

add_library(Scripting ${SCRIPTING_FILES})
set_property(TARGET Scripting PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SCRIPTING_FILES})

target_include_directories(Scripting PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core ${MONO_INCLUDE_PATH})

target_precompile_headers(Scripting PRIVATE 
							${ENGINE_SOURCE_DIRECTORY}/Core/Common/SC_Base.h
							SSC_Defines.h
							)