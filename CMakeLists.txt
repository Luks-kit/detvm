cmake_minimum_required(VERSION 3.16)
enable_testing()
project(detvm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})



add_subdirectory(vm)
add_subdirectory(asm)

add_test(
    NAME end_to_end_1
    COMMAND bash -c "
        build/detasm docs/examples/detasm/factorial.detasm ./factorial.dto &&
        build/detasm docs/examples/detasm/main.detasm ./main.dto &&
        build/detld main.dto factorial.dto factorial.dvm &&
        build/detdisasm factorial.dvm > testout.txt &&
        diff testout.txt docs/tests/expectedout.txt &&
        build/detvm factorial.dvm > testvmout.txt &&
        diff testvmout.txt docs/tests/exepectedvmout.txt
        "
)