include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/props-def/include)

find_package ( PCRE )

# Add executable called "props" that is built from the source files
# "props.cc",etc. The extensions are automatically found.
file(GLOB EXEC_SOURCES "*.cc")
file(GLOB EXEC_HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")
add_executable (props ${EXEC_SOURCES} ${EXEC_HEADERS})

# Link the executable to the Props default library. Since the Props default library has
# public include directories we will use those link directories when building
# props
target_link_libraries (props props_def ${PCRE_LIBRARIES})
