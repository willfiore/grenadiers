cmake_minimum_required (VERSION 3.10)
project (platformer)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# C++14
set (CMAKE_CXX_STANDARD 14)

# Debug
set(CMAKE_BUILD_TYPE Debug)

# Headers
include_directories(include)
# Src
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Executable
add_executable(platformer ${SOURCES})

# Link libraries
target_link_libraries(platformer glfw GL)
