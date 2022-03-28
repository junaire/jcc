cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

# Give me warnings as much as possible
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wnon-virtual-dtor -pedantic -Wcast-align -Wnull-dereference")

project(jcc)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)
