#
# CMakeLists.txt
#
# Created on 2016-06-05.
# Copyright GeniALE 2016.
# Marc-Andre Guimond
#
# This file is encoded as UTF-8.

# TODO: CMake on BBB is 2.8.9 (should be 2.8.12).
cmake_minimum_required (VERSION 2.8.9)

project (BlueZdev)

# Set bin variables.
set (VersionMaj "1")
set (VersionMin "0")
set (BINARY_NAME "bluezapi-${VersionMaj}.${VersionMin}")

# Set project paths.
set (REPOS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set (SHARED_DIR ${REPOS_ROOT}/Commun)
# Include compiler options.
include (${REPOS_ROOT}/Commun/CMake/CompilerOptions.cmake)

# Remove -std=c99, cannot support some functions where storage size of ‘sa’ isn’t known & ‘SA_NOCLDSTOP’ undeclared.
string (REGEX REPLACE "-std=c99" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

# Source file paths relative to source directory.
set ( BBCONTROLLER_SOURCE_FILES
      utils.c
      att.c
      btio.c
      gattrib.c
      gatt.c
      ble_api.c )

add_executable ( ${BINARY_NAME}
                 ${BBCONTROLLER_SOURCE_FILES} )

find_library (LIB_BLUETOOTH bluetooth)
find_library (LIB_GLIB glib-2.0)

# Find include directories for glib and add them directly to compiler flags.
execute_process ( COMMAND pkg-config --cflags-only-I glib-2.0
                  OUTPUT_VARIABLE GLIB_INCLUDES
                  OUTPUT_STRIP_TRAILING_WHITESPACE )
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GLIB_INCLUDES}")

target_link_libraries ( ${BINARY_NAME}
                        ${LIB_BLUETOOTH}
                        ${LIB_GLIB} )

