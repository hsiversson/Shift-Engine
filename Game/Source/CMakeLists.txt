##################################
# Gather all .cpp and .h files in current directory
file(GLOB_RECURSE GAME_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")

##################################
# Launchable project (outputs .exe)
add_executable(ExampleGame WIN32 ${GAME_FILES})
set_property(TARGET ExampleGame PROPERTY FOLDER "3. Executables") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GAME_FILES})


##################################
# Target Settings
target_include_directories(ExampleGame PRIVATE 
                            ${ENGINE_SOURCE_DIRECTORY}/Modules
                            ${ENGINE_SOURCE_DIRECTORY}/Core)
                            
target_link_libraries(ExampleGame
                        GameFramework	
                        Common
                        InputOutput
                        Memory
						EngineTypes
                        Platform)

target_precompile_headers(ExampleGame PRIVATE ${ENGINE_SOURCE_DIRECTORY}/Core/Common/SC_Base.h)