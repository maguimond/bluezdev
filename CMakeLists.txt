#
# CMakeLists.txt
#
# Created on 2015-09-11.
# Copyright GeniALE 2015.
# Marc-Andre Guimond
#
# This file is encoded as UTF-8.

cmake_minimum_required(VERSION 2.8.9)

project(BluetoothLECommunicator)

set(BINARY_NAME "bluecomm")
set(REPOS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Include compiler options.
include(${REPOS_ROOT}/Commun/CMake/CompilerOptions.cmake)

# Source file paths relative to source directory.
set( BLUECOMM_SOURCE_FILES
     src/main.c
     src/utils.c )

set( BLUECOMM_HEADER_FILES
     include/utils.h )

include_directories(include)

find_library(LIB_BLUETOOTH bluetooth)

add_executable(${BINARY_NAME} ${BLUECOMM_SOURCE_FILES} ${BLUECOMM_HEADER_FILES})
target_link_libraries(${BINARY_NAME} ${LIB_BLUETOOTH})
#target_link_libraries(${BINARY_NAME} /usr/lib/libbluetooth.so)
#target_link_libraries(${BINARY_NAME} /usr/lib/x86_64-linux-gnu/libbluetooth.so)})

