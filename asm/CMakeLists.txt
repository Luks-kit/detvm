cmake_minimum_required(VERSION 3.16)
project(detasm LANGUAGES CXX)
project(detdisasm LANGUAGES CXX)
project(detld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
add_executable(detasm
    src/asmmain.cpp
    src/assembler.cpp
    src/writer.cpp
    src/helpers.cpp
    src/constant_pool.cpp
)

target_include_directories(detasm PRIVATE ../inc)

add_executable(detld
    src/ldmain.cpp
    src/linker.cpp
    src/writer.cpp
    src/reader.cpp
    src/constant_pool.cpp
)

target_include_directories(detld PRIVATE ../inc)


# --- Disassembler (optional tool)
add_executable(detdisasm
    src/disasm.cpp
    src/constant_pool.cpp
)

target_include_directories(detdisasm PRIVATE ../inc)
