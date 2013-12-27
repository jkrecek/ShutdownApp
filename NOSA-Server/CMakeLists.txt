set(TARGET_NAME NOSA-Server)

set(BUILD_CURL_EXE false)
set(BUILD_CURL_TESTS false)
set(CURL_STATICLIB true)
add_definitions(-DCURL_STATICLIB)

add_subdirectory(dep/curl)

set(LIBCURL_DIR ${PROJECT_SOURCE_DIR}/dep/curl)

aux_source_directory(src SRC_LIST)
add_executable(${TARGET_NAME} ${SRC_LIST})

find_path(LIBCURL_INCLUDE_DIR curl/curl.h HINTS ${LIBCURL_DIR}/include)
set(INCLUDE_DIR ${INCLUDE_DIR} ${LIBCURL_INCLUDE_DIR})

include_directories(${INCLUDE_DIR})
add_dependencies(${TARGET_NAME} libcurl)
add_dependencies(${TARGET_NAME} NOSA-shared)

if(WIN32)
    set(LIBS ${LIBS} ws2_32)
    set(LIBS ${LIBS} iphlpapi)
endif()
set(LIBS ${LIBS} libcurl)
set(LIBS ${LIBS} NOSA-shared)

target_link_libraries(${TARGET_NAME} ${LIBS})

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

install_targets(/bin ${TARGET_NAME})
