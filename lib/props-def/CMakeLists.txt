# External packages
find_package ( Threads )
find_package ( PCRE )

# Includes
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${PCRE_INCLUDE_DIRS})


# Create a library called "props-def" which includes the source file "props_reader.cxx".
# The extension is already found. Any number of sources could be listed here.
add_library (props_def ${CMAKE_CURRENT_SOURCE_DIR}/src/props_reader.cc ${CMAKE_CURRENT_SOURCE_DIR}/src/props_file_tracker.cc ../../src/props_search_result.cc ../../include/props_search_result.h)

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
target_include_directories (props_def PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
