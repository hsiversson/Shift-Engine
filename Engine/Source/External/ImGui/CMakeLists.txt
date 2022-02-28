file(GLOB_RECURSE IMGUI_FILES CONFIGURE_DEPENDS "*.cpp" "*.h" "*.c")

add_library(ImGui STATIC ${IMGUI_FILES} ${IMGUIZMO_FILES})
set_property(TARGET ImGui PROPERTY FOLDER "0. External") 

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${IMGUI_FILES})

target_include_directories(ImGui PRIVATE 
                            ${ENGINE_SOURCE_DIRECTORY}/Core
							${ENGINE_EXTERNAL_DIRECTORY}/ImGui
                          )

if(MSVC)
  target_compile_options(ImGui PRIVATE /W3)
else()
  target_compile_options(ImGui PRIVATE -Wall -Wpedantic)
endif()