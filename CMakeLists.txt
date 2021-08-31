cmake_minimum_required(VERSION 3.21.0)
project(
  punky
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(
  punky
  src/main.cpp
  src/readline.cpp
  src/utils.cpp
  src/Lexer.cpp
  src/Token.cpp
  src/ast.cpp
  src/Parser.cpp
  src/Object.cpp
  src/Evaluator.cpp)
