project(ShutdownServer)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_BUILD_TYPE "Release")
set(EXECUTABLE_NAME "ShutdownServer")

include_directories(${PROJECT_SOURCE_DIR}/include)
FIND_PATH(CURL_INCLUDE_DIR NAMES include/curl/curl.h)
FIND_LIBRARY(CURL_LIBRARY NAMES curl)

aux_source_directory(src SRC_LIST)
add_executable(${EXECUTABLE_NAME} ${SRC_LIST})

SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "-Wl,--no-undefined -static -static-libgcc -static-libstdc++ -Wl,-O1 -Wl,--as-needed -Wl,--sort-common -s")
target_link_libraries(${EXECUTABLE_NAME} ws2_32 iphlpapi ${CURL_LIBRARY})

install(TARGETS ${EXECUTABLE_NAME} EXPORT Config
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib/static)
install(EXPORT Config DESTINATION build/config)
