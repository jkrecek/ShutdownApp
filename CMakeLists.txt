project(NOSA)
cmake_minimum_required(VERSION 2.8)
#set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(NOSA-shared)
add_subdirectory(NOSA-RemoteServer)
add_subdirectory(NOSA-Server)
