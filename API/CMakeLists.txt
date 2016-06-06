#
# CMakeLists.txt
#
# Created on 2016-06-05.
# Copyright GeniALE 2016.
# Marc-Andre Guimond
#
# This file is encoded as UTF-8.

project(BlueZdev)
# TODO: CMake on BBB is 2.8.9 (should be 2.8.12).
cmake_minimum_required(VERSION 2.8.9)

# Set bin variables.
set ( BINARY_NAME "bluezapi" )
set ( VersionMaj "1" )
set ( VersionMin "0" )

# Set cross compiler.
set ( CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++ )

set ( REPOS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../.. )
# Specifying project repository path if built from an IDE which project files are not within the project repo.
set ( PROJECT_REPO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.. )
set ( SHARED_DIR ${REPOS_ROOT}/Commun )
# Include compiler options.
include ( ${REPOS_ROOT}/Commun/CMake/CompilerOptions.cmake )

# Source file paths relative to source directory.
set ( BBCONTROLLER_SOURCE_FILES
      utils.c
      att.c
      btio.c
      gattrib.c
      gatt.c
      ble_api.c )

# Get Git info
execute_process ( COMMAND git -C ${PROJECT_REPO_DIR} rev-parse --short HEAD
                  OUTPUT_VARIABLE GIT_GLOBAL_REV
                  OUTPUT_STRIP_TRAILING_WHITESPACE )
execute_process ( COMMAND git -C ${PROJECT_REPO_DIR} rev-list --count HEAD
                  OUTPUT_VARIABLE GIT_LOCAL_REV
                  OUTPUT_STRIP_TRAILING_WHITESPACE )

# Display Git info
message ( "Global rev: " ${GIT_GLOBAL_REV} )
message ( "Local rev: " ${GIT_LOCAL_REV} )

# Pass the pre-processor to the compiler for the version constants.
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DkVersionGlobalVCS=${GIT_GLOBAL_REV} -DkVersionLocalVCS=${GIT_LOCAL_REV} -DkVersionMaj${VersionMaj} -DkVersionMin${VersionMin} " )

find_library ( LIB_BLUETOOTH bluetooth )
find_library ( LIB_GLIB glib-2.0 )
include_directories ( /usr/include/glib-2.0
                      /usr/lib/i386-linux-gnu/glib-2.0/include )

add_executable ( ${BINARY_NAME} ${BBCONTROLLER_SOURCE_FILES} )
target_link_libraries ( ${BINARY_NAME} ${LIB_BLUETOOTH} ${LIB_GLIB} )

