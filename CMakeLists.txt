cmake_minimum_required(VERSION 3.22)
project(ZDK)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(3rd)

add_subdirectory(cc)
add_subdirectory(cparse)
