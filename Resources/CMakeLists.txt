# Resources CMakeLists.txt

# Set minimum verison
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Get directory name to set lib name
get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Get project header files
file(GLOB_RECURSE TARGET_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

# Get project source files
file(GLOB_RECURSE TARGET_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Generate visual studio filters
source_group("Header" FILES ${TARGET_HEADER_FILES})	
source_group("Source" FILES ${TARGET_SOURCE_FILES})

# Create variable for all project files
set(TARGET_FILES ${TARGET_HEADER_FILES} ${TARGET_SOURCE_FILES})

# Generate static lib
add_library(${TARGET_NAME} STATIC)

# Add source files to target library
target_sources(${TARGET_NAME} PRIVATE ${TARGET_FILES})


# Add LibMath as include dir and give access to its own header directory
target_include_directories(${TARGET_NAME}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Header
	PRIVATE ${LIBMATH_INCLUDE_DIR}
)

# Link against necessary libs
target_link_libraries(${TARGET_NAME} PRIVATE ${DEPENDENCIES_LIBRARY})
target_link_libraries(${TARGET_NAME} PRIVATE ${LIBMATH_LIBRARY})

# Set exposed variable for other project to link against it
set(RESOURCE_LIBRARY ${TARGET_NAME} PARENT_SCOPE)