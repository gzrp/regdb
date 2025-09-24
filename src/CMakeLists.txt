add_subdirectory(core)
add_subdirectory(registry)
add_subdirectory(functions)

set(EXTENSION_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/regdb_extension.cpp ${EXTENSION_SOURCES}
        PARENT_SCOPE)