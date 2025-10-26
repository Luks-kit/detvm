
project(detvm LANGUAGES CXX)

file(GLOB VM_SRC
    src/*.cpp
)
file(GLOB VM_INC
    ../inc/*.hpp
)



add_executable(detvm ${VM_SRC} ${VM_INC})
target_include_directories(detvm PRIVATE ../inc)
