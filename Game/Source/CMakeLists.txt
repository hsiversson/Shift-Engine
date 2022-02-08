
##################################
# Gather all .cpp and .h files in current directory
file(GLOB GAME_FILES "*.cpp" "*.h")

##################################
# Launchable project (outputs .exe)
add_executable(ExampleGame WIN32 ${GAME_FILES})
set_property(TARGET ExampleGame PROPERTY FOLDER "3. Executables") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GAME_FILES})


##################################
# Target Settings
target_include_directories(ExampleGame PUBLIC ${ENGINE_SOURCE_DIRECTORY}/Modules/GameFramework)

target_link_libraries(ExampleGame GameFramework)
