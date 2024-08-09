# ~ LibMath/LibMath
cmake_minimum_required(VERSION 3.24 FATAL_ERROR) # PATH_EQUAL

get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

if(${CMAKE_CURRENT_SOURCE_DIR} PATH_EQUAL ${CMAKE_SOURCE_DIR})
	# Create project if build directly
	set(CMAKE_CXX_STANDARD 20)
	set(CMAKE_CXX_STANDARD_REQUIRED True)

	project(${TARGET_NAME} LANGUAGES CXX)
endif()


# ~ Sources
file(GLOB_RECURSE TARGET_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.inl)

file(GLOB_RECURSE TARGET_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cc # C with classe
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
	${CMAKE_CURRENT_SOURCE_DIR}/*.c++)

file(GLOB_RECURSE TARGET_EXTRA_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*.txt
	${CMAKE_CURRENT_SOURCE_DIR}/*.md)

set(TARGET_FILES ${TARGET_HEADER_FILES} ${TARGET_SOURCE_FILES} ${TARGET_EXTRA_FILES})

source_group("Header" FILES ${TARGET_HEADER_FILES}) # generate visual studio filter
source_group("Source" FILES ${TARGET_SOURCE_FILES})
source_group("Extra" FILES	${TARGET_EXTA_FILES})

set(TARGET_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Header)


# ~ Static library
add_library(${TARGET_NAME} STATIC)

target_sources(${TARGET_NAME} PRIVATE ${TARGET_FILES})

target_include_directories(${TARGET_NAME} PUBLIC ${TARGET_INCLUDE_DIR}/LibMath)

if(MSVC)
	target_compile_options(${TARGET_NAME} PRIVATE /W3 /WX)
else()
	message("not using MSVC")
endif()


# ~ Exposed variables
set(LIBMATH_LIBRARY ${TARGET_NAME} PARENT_SCOPE)
set(LIBMATH_INCLUDE_DIR ${TARGET_INCLUDE_DIR} PARENT_SCOPE)
