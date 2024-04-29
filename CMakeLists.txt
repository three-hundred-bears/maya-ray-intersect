cmake_minimum_required(VERSION 3.22.1)

set(PROJECT_NAME rayIntersect)
project(${PROJECT_NAME})

include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

include_directories("${CMAKE_SOURCE_DIR}")

set(SOURCE_FILES 
    rayIntersectFty.cpp 
    rayIntersectCmd.cpp 
    rayIntersectNode.cpp 
    rayIntersectMain.cpp 
    rayIntersectFty.h 
    rayIntersectCmd.h 
    rayIntersectNode.h
)

set(LIBRARIES OpenMaya Foundation)

build_plugin()
