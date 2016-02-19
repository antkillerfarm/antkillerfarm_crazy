#Set the name and the supported language of the project
project(minibrowser C)
# Set the minimum version of cmake required to build this project
cmake_minimum_required(VERSION 2.6)
# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(WEBKITGTK3 REQUIRED webkitgtk-3.0)
# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${WEBKITGTK3_INCLUDE_DIRS})
message(status ":WEBKITGTK3_INCLUDE_DIRS = ${WEBKITGTK3_INCLUDE_DIRS}")
link_directories(${WEBKITGTK3_LIBRARY_DIRS})
message(status ":WEBKITGTK3_LIBRARY_DIRS = ${WEBKITGTK3_LIBRARY_DIRS}")
# Add other flags to the compiler
add_definitions(${WEBKITGTK3_CFLAGS_OTHER})
message(status ":WEBKITGTK3_CFLAGS_OTHER = ${WEBKITGTK3_CFLAGS_OTHER}")
# Add an executable compiled from hello.c
add_executable(minibrowser minibrowser.c)
# Link the target to the GTK+ libraries
target_link_libraries(minibrowser ${WEBKITGTK3_LIBRARIES})
message(status ":WEBKITGTK3_LIBRARIES = ${WEBKITGTK3_LIBRARIES}")

message(status ":pkgs_cflags = ${PKGS_CFLAGS}")