set(TARGET_NAME NOSA)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src PARENT_SCOPE)

aux_source_directory(src SRC_LIST)

#add source-less headers
set(SRC_LIST ${SRC_LIST}
    src/defines.h)

if(BUILD_PROJECT_STATIC)
    add_library(${TARGET_NAME} STATIC ${SRC_LIST})
else()
    add_library(${TARGET_NAME} SHARED ${SRC_LIST})
endif()

if(WIN32)
    set(LIBS ${LIBS} ws2_32)
    set(LIBS ${LIBS} iphlpapi)
else()
    set(LIBS ${LIBS} pthread)
endif()

target_link_libraries(${TARGET_NAME} ${LIBS})

if(NOT BUILD_PROJECT_STATIC)
    install(TARGETS ${TARGET_NAME} DESTINATION .)
endif()
