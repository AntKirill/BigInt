cmake_minimum_required(VERSION 3.5)
project(BigInt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES big_integer.cpp big_integer.h)
add_executable(BigInt ${SOURCE_FILES})