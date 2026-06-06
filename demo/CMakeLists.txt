# Add source to this project's executable.
cmake_minimum_required(VERSION 3.16)
project(Demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(demo ${SOURCES})

target_include_directories(demo
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(demo PRIVATE sole)
