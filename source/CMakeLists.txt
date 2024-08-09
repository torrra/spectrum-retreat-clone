# Root project CMakeLists.txt

cmake_minimum_required (VERSION 3.25 FATAL_ERROR)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)

project(ProjetFinAnnee)

# External ependencies
add_subdirectory(LibMath)
add_subdirectory(DataStructures)
add_subdirectory(External)

# ModernOpenGL-related libs
add_subdirectory(Resources)
add_subdirectory(Physics)
add_subdirectory(LowRenderer)

# Game executable project
add_subdirectory(SpectrumAsylum)

if (MSVC)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SpectrumAsylum)
endif()