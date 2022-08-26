file(GLOB_RECURSE RENDERCORE_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_library(RenderCore STATIC ${RENDERCORE_FILES})
set_property(TARGET RenderCore PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RENDERCORE_FILES})
add_definitions(-D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)

target_include_directories(RenderCore PRIVATE 
							${ENGINE_SOURCE_DIRECTORY}/Core
							${D3D12_INCLUDE_PATH}
							${DIRECTSTORAGE_INCLUDE_PATH}
							${DXC_INCLUDE_PATH}
							${WINPIXEVENTRUNTIME_INCLUDE_PATH}
							${VULKAN_WIN64_INCLUDE_PATH}
							${IMGUI_INCLUDE_PATH}
							${NVAPI_INCLUDE_PATH}
							${AGS_INCLUDE_PATH}
							${AFTERMATH_INCLUDE_PATH})
							
target_precompile_headers(RenderCore PRIVATE 
							${ENGINE_SOURCE_DIRECTORY}/Core/Common/SC_Base.h
							Defines/SR_PlatformHeader.h
							Defines/SR_RenderEnums.h
							Defines/SR_Format.h
							ShaderCompiler/SR_ShaderCompileStructs.h
							Interface/SR_RenderTypes.h
							Interface/SR_RenderStates.h
							Interface/SR_RenderDevice.h
							)