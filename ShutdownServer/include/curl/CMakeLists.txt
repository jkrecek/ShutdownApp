aux_source_directory(. libcurl_SRC)
add_library(libcurl STATIC IMPORTED ${libcurl_SRC} )
SET_TARGET_PROPERTIES(libcurl PROPERTIES LINKER_LANGUAGE CXX)
