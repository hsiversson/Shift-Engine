file(GLOB_RECURSE EDITOR_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_library(Editor ${EDITOR_FILES})
set_property(TARGET Editor PROPERTY FOLDER "3. Editor") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${EDITOR_FILES})

target_include_directories(Editor PRIVATE 
							${ENGINE_SOURCE_DIRECTORY}/Modules 
							${ENGINE_SOURCE_DIRECTORY}/Core 
							${IMGUI_INCLUDE_PATH}
							${ASSIMP_INCLUDE_PATH})

target_precompile_headers(Editor PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core/Common/SC_Base.h ${ENGINE_SOURCE_DIRECTORY}/Editor/SED_WidgetUtils.h ${ENGINE_SOURCE_DIRECTORY}/Editor/SED_Widgets.h ${IMGUI_INCLUDE_PATH}/imgui.h)