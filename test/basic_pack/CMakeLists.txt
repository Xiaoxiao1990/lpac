get_filename_component(CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(MODULE_NAME ${CURRENT_FOLDER})

file(GLOB SOURCES "*.c")

# list(REMOVE_ITEM SOURCES "main.c")

include_directories(.)

add_library(${MODULE_NAME} ${SOURCES})
target_link_libraries(${MODULE_NAME} pthread crypto parson)

target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
