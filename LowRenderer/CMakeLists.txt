# LowRenderer CMakeLists.txt

# Set minimum version to 3.25
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Get directory name to set lib name
get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Get all header files
file(GLOB_RECURSE TARGET_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

# Get all source files
file(GLOB_RECURSE TARGET_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Create filters to seperate headers & source files
source_group("Header" FILES ${TARGET_HEADER_FILES})
source_group("Source" FILES ${TARGET_SOURCE_FILES})

# Create variable for project files
set(TARGET_FILES ${TARGET_HEADER_FILES} ${TARGET_SOURCE_FILES})

# Generate static lib
add_library(${TARGET_NAME} STATIC)

# Add soource files to target library
target_sources(${TARGET_NAME} PRIVATE ${TARGET_FILES})

# Add LibMath as include dir and give access to its own header directory
target_include_directories(${TARGET_NAME}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Header
	PRIVATE ${LIBMATH_INCLUDE_DIR}
)

# Add the following libraries into the low renderer project
target_link_libraries(${TARGET_NAME} PRIVATE ${DEPENDENCIES_LIBRARY})
target_link_libraries(${TARGET_NAME} PRIVATE ${LIBMATH_LIBRARY})
target_link_libraries(${TARGET_NAME} PRIVATE ${RESOURCE_LIBRARY})
target_link_libraries(${TARGET_NAME} PRIVATE ${DATASTRUCTURES_LIBRARY})
target_link_libraries(${TARGET_NAME} PRIVATE ${PHYSICS_LIBRARY})

# Set exposed variable for other project to link against it
set(LOWRENDERER_LIBRARY ${TARGET_NAME} PARENT_SCOPE)

