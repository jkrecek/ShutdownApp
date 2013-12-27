set(TARGET_NAME NOSA-shared)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src PARENT_SCOPE)

aux_source_directory(src SRC_LIST)
add_library(${TARGET_NAME} STATIC ${SRC_LIST})

if(WIN32)
    set(LIBS ${LIBS} ws2_32)
    set(LIBS ${LIBS} iphlpapi)
endif()

target_link_libraries(${TARGET_NAME} ${LIBS})
