# Root project CMakeLists.txt

cmake_minimum_required (VERSION 3.25 FATAL_ERROR)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)

project(Platformer3D)

add_subdirectory(source)