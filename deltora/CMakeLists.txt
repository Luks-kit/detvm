project(deltorac LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(deltorac 
                src/main.cpp
                src/lexer.cpp
                src/parser.cpp
                src/emit.cpp
                src/ast.cpp
                src/typecheck.cpp)

target_include_directories(deltorac PRIVATE inc)