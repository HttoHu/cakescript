cmake_minimum_required(VERSION 3.14)
project(cake)

include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED)
add_compile_options("-g")
# fetch fmt library
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
  GIT_SHALLOW ON)
FetchContent_MakeAvailable(fmt)

set (cake_sources 
  "src/context.cpp"
  "src/lexer.cpp"
  "src/parser/parser.cpp"
  "src/parser/expr.cpp"
  "src/parser/decl.cpp"
  "src/object.cpp"
)

include_directories(include
  include/parser
)


add_library(libcake STATIC ${cake_sources})
target_link_libraries(
  libcake 
  fmt::fmt 
)
target_include_directories(libcake
  PUBLIC 
  "include"
)
# build cake
add_executable(
  ${PROJECT_NAME} 
  "./src/main.cpp"
)

target_link_libraries(${PROJECT_NAME} 
PRIVATE 
  libcake
)

add_subdirectory(test)