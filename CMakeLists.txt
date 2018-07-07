cmake_minimum_required (VERSION 3.10)
project (grenadiers)

find_package(Freetype REQUIRED)

# Compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# C++17
set (CMAKE_CXX_STANDARD 17)

# Debug
set(CMAKE_BUILD_TYPE Debug)

# Include
include_directories(${FREETYPE_INCLUDE_DIRS})

# Src
file(GLOB_RECURSE SOURCES "src/*.c*")

# Executable
add_executable(grenadiers ${SOURCES})

# Link libraries
target_link_libraries(grenadiers glfw dl ${FREETYPE_LIBRARIES})
