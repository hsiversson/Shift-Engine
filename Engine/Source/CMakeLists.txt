
##################################
# Engine Core sub-directories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/Platform)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/EngineTypes)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/Memory)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/InputOutput)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/Common)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/Render)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Core/Network)

##################################
# Engine Modules sub-directories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Modules/Audio)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Modules/Graphics)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Modules/GameFramework)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Modules/Physics)