# Spectrum Asylum CMakeLists.txt

# Check CMake version & cause fatal error if lower than 3.25
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Get directory name to set lib name
get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Get .gitIgnore file
source_group("Extras" FILES ${CMAKE_SOURCE_DIR}/.gitignore)

# Get shader files
file(GLOB_RECURSE SHADERS
    ${CMAKE_CURRENT_BINARY_DIR}/shaders/*
)

# Get asset files
file(GLOB_RECURSE ASSETS
    ${CMAKE_CURRENT_BINARY_DIR}/assets/*
)

file(GLOB_RECURSE LEVELS
    ${CMAKE_CURRENT_BINARY_DIR}/scenes/*.level
)

source_group("Levels" FILES ${LEVELS})

# Generate Visual Studio filters for assets & shaders
source_group("Assets" FILES ${ASSETS})
source_group("Shaders" FILES ${SHADERS})

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

# Generate Visual Studio filters
source_group("Header" FILES ${TARGET_HEADER_FILES})
source_group("Source" FILES ${TARGET_SOURCE_FILES})

# Create variable for all project files
set(TARGET_FILES ${TARGET_HEADER_FILES} ${TARGET_SOURCE_FILES} ${SHADERS} ${LEVELS})

# Create executable file
add_executable(${TARGET_NAME})

# Set target source files
target_sources(${TARGET_NAME} PRIVATE ${TARGET_FILES} ${CMAKE_SOURCE_DIR}/.gitignore)

# Get game header files
target_include_directories(${TARGET_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Header
    PRIVATE ${LIBMATH_INCLUDE_DIR}
)

# Get all dependencies for game
target_link_libraries(${TARGET_NAME}
	PRIVATE ${LIBMATH_LIBRARY}
	PRIVATE ${PHYSICS_LIBRARY}
	PRIVATE ${DEPENDENCIES_LIBRARY}
	PRIVATE ${RESOURCE_LIBRARY}
	PRIVATE ${LOWRENDERER_LIBRARY}
	PRIVATE ${DATASTRUCTURES_LIBRARY}
)
