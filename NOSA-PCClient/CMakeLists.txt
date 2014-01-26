set(TARGET_NAME NOSA-PCClient)

aux_source_directory(src SRC_LIST)
add_executable(${TARGET_NAME} WIN32 ${SRC_LIST})
#add_executable(${TARGET_NAME} ${SRC_LIST})

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

install(TARGETS ${TARGET_NAME} DESTINATION .)
