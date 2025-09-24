add_subdirectory(core)
add_subdirectory(registry)
add_subdirectory(functions)
add_subdirectory(custom_parser)

set(EXTENSION_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/regdb_extension.cpp ${EXTENSION_SOURCES}
        PARENT_SCOPE)