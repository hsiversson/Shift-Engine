
##################################
# Gather all .cpp and .h files in current directory
file(GLOB GAME_FILES "*.cpp" "*.h")

##################################
# Launchable project (outputs .exe)
add_executable(ExampleGame WIN32 ${GAME_FILES})

