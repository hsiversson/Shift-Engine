##################################
# Required CMake version
cmake_minimum_required(VERSION 3.18)

##################################
# Solution Name + Version
project(ShiftEngine VERSION 1.0)

##################################
# Set various C++ config variables
set(CMAKE_CXX_STANDARD 17)                  ## C++17 Standard
set(CMAKE_CXX_STANDARD_REQUIRED True)       ## Enforce the C++17 Standard

##################################
# Repository folder structure setup
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Bin)               ## Where to put the .exe files etc.
set(ENGINE_SOURCE_DIRECTORY ${PROJECT_SOURCE_DIR}/Engine/Source)            ## Where is the engine source code located
set(GAME_SOURCE_DIRECTORY ${PROJECT_SOURCE_DIR}/Samples/ExampleGame/Source) ## Where is the game source code located
set(ENGINE_EXTERNAL_DIRECTORY ${ENGINE_SOURCE_DIRECTORY}/External)

##################################
# Misc settings
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

##################################
## Build Options
option(BUILD_EDITOR "Build engine with editor" OFF)
option(BUILD_EXAMPLE_GAME "Build example game" OFF)

##################################
# Global Defines
add_definitions(
    -DUNICODE 
    -D_UNICODE
    -DIMGUI_USER_CONFIG="${ENGINE_SOURCE_DIRECTORY}/Core/RenderCore/ImGui/SR_ImGuiConfig.h"
)

if (BUILD_EDITOR)
    add_definitions(-DENABLE_EDITOR)
endif()

##################################
# Compiler options
if (MSVC)
    add_compile_options(-W4 -WX)                            ## Adds warning level 4 and enables warnings as errors
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)    ## GCC/CLANG version of warning level 4 and warnings as errors
endif()

##################################
# 3rd Party Libraries

## Assimp
set(ASSIMP_SDK_PATH "${ENGINE_EXTERNAL_DIRECTORY}/Assimp")
set(ASSIMP_BIN_PATH "${ASSIMP_SDK_PATH}/bin")
set(ASSIMP_LIB_PATH "${ASSIMP_SDK_PATH}/lib")
set(ASSIMP_INCLUDE_PATH "${ASSIMP_SDK_PATH}/include")
set(ASSIMP_LIB "${ASSIMP_LIB_PATH}/assimp-vc143-mt.lib")

## DirectX 12 Agility SDK
set(D3D12_SDK_PATH "${ENGINE_EXTERNAL_DIRECTORY}/D3D12")
set(D3D12_BIN_PATH "${D3D12_SDK_PATH}/bin")
set(D3D12_INCLUDE_PATH "${D3D12_SDK_PATH}/include")

## DirectX Shader Compiler
set(DXC_SDK_PATH "${ENGINE_EXTERNAL_DIRECTORY}/DirectXShaderCompiler")
set(DXC_BIN_PATH "${DXC_SDK_PATH}/bin")
set(DXC_LIB_PATH "${DXC_SDK_PATH}/lib")
set(DXC_INCLUDE_PATH "${DXC_SDK_PATH}/include")
set(DXC_LIB "${DXC_LIB_PATH}/dxcompiler.lib")

## ImGui
set(IMGUI_SDK_PATH "${ENGINE_EXTERNAL_DIRECTORY}/ImGui")
set(IMGUI_INCLUDE_PATH "${IMGUI_SDK_PATH}")

## Gather all SDKs into one variable
set(VENDOR_SDK_LIBS ${ASSIMP_LIB} ${DXC_LIB})

##################################
# Utility functions

function(LinkVendorLibraries aTarget)
    target_link_libraries(${aTarget} ${VENDOR_SDK_LIBS})
endfunction()

function(ConfigTargetPostBuildLibInstall aTarget)

    ## DirectX 12 Agility SDK
    set(D3D12_SDK_LAYERS_DLL d3d12SDKLayers.dll)
    set(D3D12_CORE_DLL D3D12Core.dll)
    add_custom_command(
        TARGET ${aTarget} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy "${D3D12_BIN_PATH}/${D3D12_SDK_LAYERS_DLL}" "$<TARGET_FILE_DIR:${aTarget}>/${D3D12_SDK_LAYERS_DLL}"
        COMMAND ${CMAKE_COMMAND} -E copy "${D3D12_BIN_PATH}/${D3D12_CORE_DLL}" "$<TARGET_FILE_DIR:${aTarget}>/${D3D12_CORE_DLL}"
        COMMENT "Copying [${D3D12_CORE_DLL}] and [${D3D12_SDK_LAYERS_DLL}] to output directory...\n"
    )

    ## DirectX Shader Compiler
    set(DXC_DXCOMPILER_DLL dxcompiler.dll)
    set(DXC_DXIL_DLL dxil.dll)
    add_custom_command(
        TARGET ${aTarget} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy "${DXC_BIN_PATH}/${DXC_DXCOMPILER_DLL}" "$<TARGET_FILE_DIR:${aTarget}>/${DXC_DXCOMPILER_DLL}"
        COMMAND ${CMAKE_COMMAND} -E copy "${DXC_BIN_PATH}/${DXC_DXIL_DLL}" "$<TARGET_FILE_DIR:${aTarget}>/${DXC_DXIL_DLL}"
        COMMENT "Copying [${DXC_DXIL_DLL}] and [${DXC_DXCOMPILER_DLL}] to output directory...\n"
    )

    ## Assimp
    set(ASSIMP_DLL assimp-vc143-mt.dll)
    add_custom_command(
        TARGET ${aTarget} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy "${ASSIMP_BIN_PATH}/${ASSIMP_DLL}" "$<TARGET_FILE_DIR:${aTarget}>/${ASSIMP_DLL}"
        COMMENT "Copying [${DXC_DXIL_DLL}] to output directory...\n"
    )
endfunction()

##################################
# Repository sub-directories
add_subdirectory(${ENGINE_SOURCE_DIRECTORY})

if (BUILD_EXAMPLE_GAME)
    add_subdirectory(${GAME_SOURCE_DIRECTORY})
endif()