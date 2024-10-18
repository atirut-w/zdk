cmake_minimum_required(VERSION 3.22)
project(ZDK)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("3rd/argparse")
add_subdirectory("3rd/antlr4-cpp-runtime")

add_subdirectory("src")
