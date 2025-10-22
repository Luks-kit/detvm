
project(detvm_vm LANGUAGES CXX)

file(GLOB VM_SRC
    src/*.cpp
)
file(GLOB VM_INC
    inc/*.hpp
)



add_executable(vm ${VM_SRC} ${VM_INC})
target_include_directories(vm PRIVATE inc)
