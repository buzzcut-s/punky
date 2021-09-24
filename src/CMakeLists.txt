add_library(punky_interpreter)

target_include_directories(
  punky_interpreter
  PUBLIC ${punky_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(
  punky_interpreter
  PUBLIC utils.cpp
         Lexer.cpp
         Token.cpp
         ast.cpp
         Parser.cpp
         Object.cpp
         Evaluator.cpp
         Environment.cpp)

add_executable(punky_repl)
set_target_properties(punky_repl PROPERTIES OUTPUT_NAME "punky")

target_include_directories(
  punky_repl
  PUBLIC ${punky_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(punky_repl PRIVATE main.cpp readline.cpp)
target_link_libraries(punky_repl PRIVATE linenoise punky_interpreter)
