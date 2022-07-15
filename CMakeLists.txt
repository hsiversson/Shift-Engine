##################################
# Required CMake version
cmake_minimum_required(VERSION 3.18)

##################################
# Solution Name + Version
project(ShiftEngine VERSION 1.0)

##################################
# Set various C++ config variables
set(CMAKE_CXX_STANDARD 23)                  ## C++20 Standard
set(CMAKE_CXX_STANDARD_REQUIRED True)       ## Enforce the C++20 Standard

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

## AMD Ags
set(AGS_SDK_PATH "${ENGINE_EXTERNAL_DIRECTORY}/AMD/ags/ags_lib")
set(AGS_BIN_PATH "${AGS_SDK_PATH}/lib")
set(AGS_LIB_PATH "${AGS_SDK_PATH}/lib")
set(AGS_INCLUDE_PATH "${AGS_SDK_PATH}/inc")
set(AGS_LIB "${AGS_LIB_PATH}/amd_ags_x64.lib")

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

## DirectStorage SDK
set(DIRECTSTORAGE_SDK_PATH "${ENGINE_EXTERNAL_DIRECTORY}/DirectStorage")
set(DIRECTSTORAGE_BIN_PATH "${DIRECTSTORAGE_SDK_PATH}/bin")
set(DIRECTSTORAGE_INCLUDE_PATH "${DIRECTSTORAGE_SDK_PATH}/Include")
set(DIRECTSTORAGE_LIB "${DIRECTSTORAGE_BIN_PATH}/dstorage.lib")

## DirectX Shader Compiler
set(DXC_SDK_PATH "${ENGINE_EXTERNAL_DIRECTORY}/DirectXShaderCompiler")
set(DXC_BIN_PATH "${DXC_SDK_PATH}/bin")
set(DXC_LIB_PATH "${DXC_SDK_PATH}/lib")
set(DXC_INCLUDE_PATH "${DXC_SDK_PATH}/include")
set(DXC_LIB "${DXC_LIB_PATH}/dxcompiler.lib")

## ImGui
set(IMGUI_SDK_PATH "${ENGINE_EXTERNAL_DIRECTORY}/ImGui")
set(IMGUI_INCLUDE_PATH "${IMGUI_SDK_PATH}")

## Nlohmann JSON
set(NLOHMANN_JSON_SDK_PATH "${ENGINE_EXTERNAL_DIRECTORY}/NlohmannJson")
set(NLOHMANN_JSON_INCLUDE_PATH "${NLOHMANN_JSON_SDK_PATH}")

## Nvidia NvApi
set(NVAPI_SDK_PATH "${ENGINE_EXTERNAL_DIRECTORY}/Nvidia/nvapi")
set(NVAPI_LIB_PATH "${NVAPI_SDK_PATH}/amd64")
set(NVAPI_INCLUDE_PATH "${NVAPI_SDK_PATH}")
set(NVAPI_LIB "${NVAPI_LIB_PATH}/nvapi64.lib")

## Nvidia Nsight Aftermath
set(AFTERMATH_SDK_PATH "${ENGINE_EXTERNAL_DIRECTORY}/Nvidia/Aftermath")
set(AFTERMATH_BIN_PATH "${AFTERMATH_SDK_PATH}/lib")
set(AFTERMATH_LIB_PATH "${AFTERMATH_SDK_PATH}/lib")
set(AFTERMATH_INCLUDE_PATH "${AFTERMATH_SDK_PATH}/include")
set(AFTERMATH_LIB "${AFTERMATH_LIB_PATH}/GFSDK_Aftermath_Lib.x64.lib")

## WinPixEventRuntime
set(WINPIXEVENTRUNTIME_SDK_PATH "${ENGINE_EXTERNAL_DIRECTORY}/WinPixEventRuntime")
set(WINPIXEVENTRUNTIME_BIN_PATH "${WINPIXEVENTRUNTIME_SDK_PATH}/bin")
set(WINPIXEVENTRUNTIME_INCLUDE_PATH "${WINPIXEVENTRUNTIME_SDK_PATH}/Include")
set(WINPIXEVENTRUNTIME_LIB "${WINPIXEVENTRUNTIME_BIN_PATH}/WinPixEventRuntime.lib")

## Gather all SDKs into one variable
set(VENDOR_SDK_LIBS ${AGS_LIB} ${ASSIMP_LIB} ${DIRECTSTORAGE_LIB} ${DXC_LIB} ${NVAPI_LIB} ${AFTERMATH_LIB} ${WINPIXEVENTRUNTIME_LIB})

##################################
# Global Include Directories
include_directories(${NLOHMANN_JSON_INCLUDE_PATH})

##################################
# Utility functions

function(LinkVendorLibraries aTarget)
    target_link_libraries(${aTarget} ${VENDOR_SDK_LIBS})
endfunction()

function(ConfigTargetPostBuildLibInstall aTarget)

    ## AMD Ags
    set(AGS_DLL amd_ags_x64.dll)
    add_custom_command(
        TARGET ${aTarget} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy "${AGS_BIN_PATH}/${AGS_DLL}" "$<TARGET_FILE_DIR:${aTarget}>/${AGS_DLL}"
        COMMENT "Copying [${AGS_DLL}] to output directory...\n"
    )
    ## DirectX 12 Agility SDK
    set(D3D12_SDK_LAYERS_DLL d3d12SDKLayers.dll)
    set(D3D12_CORE_DLL D3D12Core.dll)
    add_custom_command(
        TARGET ${aTarget} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy "${D3D12_BIN_PATH}/${D3D12_SDK_LAYERS_DLL}" "$<TARGET_FILE_DIR:${aTarget}>/${D3D12_SDK_LAYERS_DLL}"
        COMMAND ${CMAKE_COMMAND} -E copy "${D3D12_BIN_PATH}/${D3D12_CORE_DLL}" "$<TARGET_FILE_DIR:${aTarget}>/${D3D12_CORE_DLL}"
        COMMENT "Copying [${D3D12_CORE_DLL}] and [${D3D12_SDK_LAYERS_DLL}] to output directory...\n"
    )
    ## Direct Storage
    set(DIRECTSTORAGE_DLL dstorage.dll)
    set(DIRECTSTORAGECORE_DLL dstoragecore.dll)
    add_custom_command(
        TARGET ${aTarget} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy "${DIRECTSTORAGE_BIN_PATH}/${DIRECTSTORAGE_DLL}" "$<TARGET_FILE_DIR:${aTarget}>/${DIRECTSTORAGE_DLL}"
        COMMAND ${CMAKE_COMMAND} -E copy "${DIRECTSTORAGE_BIN_PATH}/${DIRECTSTORAGECORE_DLL}" "$<TARGET_FILE_DIR:${aTarget}>/${DIRECTSTORAGECORE_DLL}"
        COMMENT "Copying [${DIRECTSTORAGE_DLL}] and [${DIRECTSTORAGECORE_DLL}] to output directory...\n"
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

    ## Nvidia Aftermath
    set(AftermathDLL GFSDK_Aftermath_Lib.x64.dll)
    set(LLVM_7_0_1_DLL llvm_7_0_1.dll)
    add_custom_command(
        TARGET ${aTarget} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy "${AFTERMATH_BIN_PATH}/${AftermathDLL}" "$<TARGET_FILE_DIR:${aTarget}>/${AftermathDLL}"
        COMMAND ${CMAKE_COMMAND} -E copy "${AFTERMATH_BIN_PATH}/${LLVM_7_0_1_DLL}" "$<TARGET_FILE_DIR:${aTarget}>/${LLVM_7_0_1_DLL}"
        COMMENT "Copying [${AftermathDLL}] and [${LLVM_7_0_1_DLL}] to output directory...\n"
    )

    ## WinPixEventRuntime
    set(WINPIXEVENTRUNTIME_DLL WinPixEventRuntime.dll)
    add_custom_command(
        TARGET ${aTarget} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy "${WINPIXEVENTRUNTIME_BIN_PATH}/${WINPIXEVENTRUNTIME_DLL}" "$<TARGET_FILE_DIR:${aTarget}>/${WINPIXEVENTRUNTIME_DLL}"
        COMMENT "Copying [${WINPIXEVENTRUNTIME_DLL}] to output directory...\n"
    )

endfunction()

##################################
# Repository sub-directories
add_subdirectory(${ENGINE_SOURCE_DIRECTORY})

if (BUILD_EXAMPLE_GAME)
    add_subdirectory(${GAME_SOURCE_DIRECTORY})
endif()