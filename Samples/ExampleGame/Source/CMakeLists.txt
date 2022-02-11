##################################
# Gather all .cpp and .h files in current directory
file(GLOB_RECURSE GAME_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")

##################################
# Launchable project (outputs .exe)
add_executable(ExampleGame WIN32 ${GAME_FILES})
set_property(TARGET ExampleGame PROPERTY FOLDER "4. Executables") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GAME_FILES})

##################################
# Target Settings
target_include_directories(ExampleGame PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Modules ${ENGINE_SOURCE_DIRECTORY}/Core)
target_precompile_headers(ExampleGame PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core/Common/SC_Base.h)
                            
target_link_libraries(ExampleGame
                        GameFramework	
                        Graphics
                        Application
                        RenderCore
                        Common
                        InputOutput
                        Memory
						EngineTypes
                        Platform
                        ImGui)
LinkVendorLibraries(ExampleGame)
ConfigTargetPostBuildLibInstall(ExampleGame)

if (BUILD_EDITOR)
    target_include_directories(ExampleGame PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Editor)
    target_link_libraries(ExampleGame Editor)
endif()