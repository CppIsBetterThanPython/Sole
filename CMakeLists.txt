cmake_minimum_required(VERSION 3.16)
project(Sole LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(sole STATIC ${SOURCES})

target_include_directories(sole
PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/detail>
    $<INSTALL_INTERFACE:include>
)

add_subdirectory("demo")
