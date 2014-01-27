set(TARGET_NAME NOSA-PCClient)

aux_source_directory(src SRC_LIST)
add_executable(${TARGET_NAME} WIN32 ${SRC_LIST})

add_dependencies(${TARGET_NAME} NOSA-shared)
include_directories(${INCLUDE_DIR})
set(LIBS ${LIBS} NOSA)
target_link_libraries(${TARGET_NAME} ${LIBS})

if(WIN32 AND BUILD_PROJECT_STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

install(TARGETS ${TARGET_NAME} DESTINATION .)
