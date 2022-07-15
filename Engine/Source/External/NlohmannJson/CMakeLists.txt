file(GLOB_RECURSE JSON_FILES CONFIGURE_DEPENDS "*.cpp" "*.h" "*.c" "*.hpp")

add_library(NlohmannJson INTERFACE ${JSON_FILES})
set_property(TARGET NlohmannJson PROPERTY FOLDER "0. External") 

target_include_directories(NlohmannJson INTERFACE ${NLOHMANN_JSON_INCLUDE_PATH})