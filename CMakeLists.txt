cmake_minimum_required(VERSION 3.10)
project(detvm)

set(CMAKE_CXX_STANDARD 17)

include_directories(inc)

add_executable(detvm src/main.cpp
                     src/ops.cpp
                     src/vm.cpp
                     src/helpers.cpp
                     src/assembler.cpp
                     src/linker.cpp)

