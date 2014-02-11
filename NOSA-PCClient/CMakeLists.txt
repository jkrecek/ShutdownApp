set(TARGET_NAME NOSA)

aux_source_directory(src SRC_LIST)

set(RES_FILES "")
if(MINGW)
    set(RES_FILES "res/resources.h" "res/icon.rc")
    set(CMAKE_RC_COMPILER_INIT windres)
    enable_language(RC)
    set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)

add_executable(${TARGET_NAME} WIN32 ${SRC_LIST} ${RES_FILES})

add_dependencies(${TARGET_NAME} NOSA-shared)
include_directories(${INCLUDE_DIR})
set(LIBS ${LIBS} libNOSA)
target_link_libraries(${TARGET_NAME} ${LIBS})

if(WIN32 AND BUILD_PROJECT_STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

install(TARGETS ${TARGET_NAME} DESTINATION .)
