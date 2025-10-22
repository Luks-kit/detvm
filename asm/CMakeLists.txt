cmake_minimum_required(VERSION 3.16)
project(detasm LANGUAGES CXX)
project(detdisasm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
add_executable(detasm
    src/main.cpp
    src/assembler.cpp
    src/linker.cpp
    src/helpers.cpp
)

target_include_directories(detasm PRIVATE inc)

# --- Disassembler (optional tool)
add_executable(detdisasm
    src/disasm.cpp
    src/helpers.cpp
)

target_include_directories(detdisasm PRIVATE inc)
