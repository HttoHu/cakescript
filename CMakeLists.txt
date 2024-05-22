cmake_minimum_required(VERSION 3.14)
project(cake)

include(FetchContent)

set(LLVM_DIR "" CACHE PATH "/usr/lib/llvm-14/lib/cmake/llvm")
find_package(LLVM REQUIRED 14)
llvm_map_components_to_libnames(llvm_all ${LLVM_TARGETS_TO_BUILD} Passes ExecutionEngine MCJIT)
message(STATUS "Architecture: ${TARGET_ARCH}" )

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
  src/context.cpp
  src/lexer.cpp
  src/utils.cpp
  src/parser/parser.cpp
  src/parser/expr.cpp
  src/parser/decl.cpp
  src/parser/loop_branch.cpp
  src/parser/function.cpp
  src/parser/symbol.cpp
  
  src/runtime/object.cpp
  src/runtime/mem.cpp
  src/runtime/callable_object.cpp
  src/runtime/intern_function.cpp

  src/lib/io.cpp
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
  include
  ${LLVM_INCLUDE_DIRS}
)
# build cake
add_executable(
  ${PROJECT_NAME} 
  "./src/main.cpp"
)

target_link_libraries(${PROJECT_NAME} 
PUBLIC 
  libcake
  ${llvm_all}
)

add_subdirectory(test)