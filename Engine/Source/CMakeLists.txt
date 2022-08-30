##################################
# Engine External sub-directories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/External/ImGui)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/External/NlohmannJson)

##################################
# Engine Core sub-directories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/Application)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/Audio)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/Common)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/EngineTypes)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/InputOutput)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/Memory)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/Network)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/Physics)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/Platform)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/RenderCore)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/FacadeGUI)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/Scripting)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/Scripting/ShiftScriptCore)

##################################
# Engine Modules sub-directories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Modules/GameFramework)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Modules/Graphics)

##################################
# Engine Editor sub-directories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Editor)