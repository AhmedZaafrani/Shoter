cmake_minimum_required(VERSION 3.21)
project(shooter)

set(CMAKE_CXX_STANDARD 14)

add_executable(shooter main.cpp)

find_library(RAYLIBRARY raylib lib)

target_link_libraries(shooter LINK_PUBLIC ${RAYLIBRARY} winmm)