
file(GLOB AUDIO_FILES "*.cpp" "*.h")

add_library(Audio ${AUDIO_FILES})
set_property(TARGET Audio PROPERTY FOLDER "1. Core") 
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${AUDIO_FILES})
