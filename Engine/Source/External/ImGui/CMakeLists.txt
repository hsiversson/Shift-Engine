file(GLOB IMGUI_FILES "*.cpp" "*.h" "*.c")
file(GLOB IMGUIZMO_FILES "./ImGuizmo/*.cpp" "./ImGuizmo/*.h")

add_library(ImGui STATIC ${IMGUI_FILES} ${IMGUIZMO_FILES})
set_property(TARGET ImGui PROPERTY FOLDER "0. External") 

set(IMGUIZMOFILES 
	./ImGuizmo/GraphEditor.cpp
	./ImGuizmo/GraphEditor.h
	./ImGuizmo/ImCurveEdit.cpp
	./ImGuizmo/ImCurveEdit.h
	./ImGuizmo/ImGradient.cpp
	./ImGuizmo/ImGradient.h
	./ImGuizmo/ImGuizmo.cpp
	./ImGuizmo/ImGuizmo.h
	./ImGuizmo/ImSequencer.cpp
	./ImGuizmo/ImSequencer.h
	./ImGuizmo/ImZoomSlider.h)
source_group("ImGuizmo" FILES ${IMGUIZMOFILES})

target_include_directories(ImGui PRIVATE 
                            ${ENGINE_SOURCE_DIRECTORY}/Core/Common
                            ${ENGINE_SOURCE_DIRECTORY}/Core/InputOutput
                            ${ENGINE_SOURCE_DIRECTORY}/Core/Memory
                            ${ENGINE_SOURCE_DIRECTORY}/Core/EngineTypes
                            ${ENGINE_SOURCE_DIRECTORY}/Core/Platform
							${ENGINE_EXTERNAL_DIRECTORY}/ImGui
                          )