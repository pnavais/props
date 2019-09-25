# External packages
find_package ( Threads )
find_package ( PCRE )

# Includes
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${PCRE_INCLUDE_DIRS})

# Create a library called "props-def" which includes the source file "props_reader.cxx".
# The extension is already found. Any number of sources could be listed here.
file(GLOB LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
file(GLOB LIB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

add_library (props_def ${LIB_SOURCES} ${LIB_HEADERS})

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
target_include_directories (props_def PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
