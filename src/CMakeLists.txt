include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/props-def/include)

find_package ( PCRE )

# Add executable called "props" that is built from the source files
# "props.cc",etc. The extensions are automatically found.
file(GLOB SOURCES "*.cc")
add_executable (props ${SOURCES} ../include/props_file_tracker.h ../include/string_utils.h string_utils.cc ../include/props_cmd.h ../include/init_exception.h ../include/exec_exception.h ../include/props_result.h ../include/props_version_cmd.h ../include/props_help_cmd.h props_help_cmd.cc ../include/props_cmd_factory.h props_cmd_factory.cc props_unknown_cmd.cc ../include/props_arg.h props_cmd.cc ../include/config_static.h)

# Link the executable to the Props default library. Since the Props default library has
# public include directories we will use those link directories when building
# props
target_link_libraries (props props_def ${PCRE_LIBRARIES})
