cmake_minimum_required(VERSION 3.8)

project(filesystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(path_demo
	path_demo.cpp
	filesystem.hpp
)
enable_testing()
add_test(tests path_demo)
