# Dependencies CMakeLists.txt

# Kill process if CMake version is too old
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Gather necessary headers
file(GLOB_RECURSE TARGET_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Header/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/Header/*.hpp
)

# Gather necessary source files
file(GLOB_RECURSE TARGET_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Gather necessary libraries
file(GLOB_RECURSE TARGET_STATIC_LIBS
	${CMAKE_CURRENT_SOURCE_DIR}/Lib/*.lib
)

# Create VS filter
source_group("Dependencies" FILES ${TARGET_FILES})

# Set variable for compilation
set(TARGET_FILES ${TARGET_HEADER_FILES} ${TARGET_SOURCE_FILES})

# Create target library
add_library(${TARGET_NAME} STATIC)

# Add sources to target
target_sources(${TARGET_NAME} PRIVATE ${TARGET_FILES})

# Set public include directory for this and other projects
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Header)

# Link against necessary libraries
target_link_libraries(${TARGET_NAME} PRIVATE ${TARGET_STATIC_LIBS})

# Set name as exposed variable to build other projects
set(DEPENDENCIES_LIBRARY ${TARGET_NAME} PARENT_SCOPE)
