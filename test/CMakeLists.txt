if(APPLE)
    set(RPATH_BINARY_PATH "@loader_path")
else()
    set(RPATH_BINARY_PATH "$ORIGIN")
endif()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} DIR_LPAC_SRCS)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/applet DIR_LPAC_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/applet/chip DIR_LPAC_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/applet/notification DIR_LPAC_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/applet/profile DIR_LPAC_SRCS)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/parson)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/parson)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/basic_pack)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/basic_pack)

add_executable(lpac_test ${DIR_LPAC_SRCS})
target_link_libraries(lpac_test euicc-drivers basic_pack)
target_include_directories(lpac_test PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> ${CMAKE_CURRENT_SOURCE_DIR}/)

add_dependencies(lpac_test version)
set_target_properties(lpac_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output"
    BUILD_RPATH "${RPATH_BINARY_PATH}"
)

if(UNIX)
    install(TARGETS lpac_test RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()
