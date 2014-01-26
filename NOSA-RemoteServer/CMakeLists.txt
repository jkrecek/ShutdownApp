set(TARGET_NAME NOSA-RemoteServer)

if(PC_NO_LOOP OR ALL_NO_LOOP)
    add_definitions(-DMODULE_NO_LOOP)
else()
    remove_definitions(-DMODULE_NO_LOOP)
endif()


aux_source_directory(src SRC_LIST)
add_executable(${TARGET_NAME} ${SRC_LIST})

add_dependencies(${TARGET_NAME} NOSA-shared)
include_directories(${INCLUDE_DIR})

if(WIN32)
    set(LIBS ${LIBS} ws2_32)
    set(LIBS ${LIBS} iphlpapi)
else()
    set(LIBS ${LIBS} pthread)
endif()

set(LIBS ${LIBS} NOSA-shared)

target_link_libraries(${TARGET_NAME} ${LIBS})

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

install(TARGETS ${TARGET_NAME} DESTINATION .)
