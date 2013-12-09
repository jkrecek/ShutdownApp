project(ShutdownServer)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_BUILD_TYPE "Release")
set(BUILD_CURL_EXE false)
set(BUILD_CURL_TESTS false)
set(CURL_STATICLIB true)
add_definitions(-DCURL_STATICLIB)

add_subdirectory(dep/curl)
set(TARGET_NAME "ShutdownServer")
set(LIBCURL_DIR ${PROJECT_SOURCE_DIR}/dep/curl)

aux_source_directory(src SRC_LIST)
add_executable(${TARGET_NAME} ${SRC_LIST})

find_path(LIBCURL_INCLUDE_DIR curl/curl.h HINTS ${LIBCURL_DIR}/include)
include_directories(${LIBCURL_INCLUDE_DIR})

add_dependencies(${TARGET_NAME} libcurl)

set(LIBS ${LIBS} ws2_32)
set(LIBS ${LIBS} iphlpapi)
set(LIBS ${LIBS} libcurl)

target_link_libraries(${TARGET_NAME} ${LIBS})

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

INSTALL_TARGETS(/bin ${TARGET_NAME})
