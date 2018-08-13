# Cmake vars
cmake_minimum_required (VERSION 3.8)
set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_SUPPRESS_REGENERATION false)
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

# Own vars
set (SOLUTION_NAME "MyProject")
set (PROJECT_NAME ${SOLUTION_NAME})
set (SOURCE_FILES_PROPERTY "source_files")
set (GROUP_PROPERTY "source_group")

# Macros
include (macros.cmake)

# Add all source files
add_sources  (
	${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
	${CMAKE_CURRENT_SOURCE_DIR}/macros.cmake
)
add_subdirectories (
	Communication
	InputSimulator
	Utils
)
get_sources (SOURCES)

# Project setup
project (${SOLUTION_NAME})
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
add_executable (${PROJECT_NAME} ${SOURCES})
handle_library_dependencies()
include_directories(.)
