file(GLOB_RECURSE GRAPHICS_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_library(Graphics ${GRAPHICS_FILES})
set_property(TARGET Graphics PROPERTY FOLDER "2. Modules") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GRAPHICS_FILES})

target_include_directories(Graphics PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core ${ENGINE_SOURCE_DIRECTORY}/Modules)
target_include_directories(Graphics SYSTEM PRIVATE ${ASSIMP_INCLUDE_PATH})
							
target_precompile_headers(Graphics PRIVATE 
							${ENGINE_SOURCE_DIRECTORY}/Core/Common/SC_Base.h
							${ENGINE_SOURCE_DIRECTORY}/Core/RenderCore/Interface/SR_RenderDevice.h
							)