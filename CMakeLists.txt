# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (props)

set(CMAKE_CXX_STANDARD 11)
#set( CMAKE_VERBOSE_MAKEFILE on )
#SET(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wformat=2 -Winit-self -Wpacked -O2 -Wp,-D_FORTIFY_SOURCE=2 -Wpointer-arith -Wlarger-than-65500 -Wmissing-declarations -Wmissing-format-attribute -Wmissing-noreturn -Wredundant-decls -Wsign-compare -Wstrict-aliasing=2 -Wswitch-enum -Wundef -Wunreachable-code -Wwrite-strings -Wuninitialized")

# Modules
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
# Recurse into the "Hello" and "Demo" subdirectories. This does not actually
# cause another cmake executable to run. The same process will walk through
# the project's entire directory structure.
add_subdirectory (lib)
add_subdirectory (src)
