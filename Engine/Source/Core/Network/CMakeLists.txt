
file(GLOB NETWORK_FILES "*.cpp" "*.h")

add_library(Network ${NETWORK_FILES})
set_property(TARGET Network PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NETWORK_FILES})