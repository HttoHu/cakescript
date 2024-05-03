cmake_minimum_required(VERSION 3.14)
project(cake)

include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED)

# fetch fmt library
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
  GIT_SHALLOW ON)
FetchContent_MakeAvailable(fmt)

include_directories(${PROJECT_SOURCE_DIR}/include)

set (cake_sources 
  "src/context.cpp"
  "src/lexer.cpp"
)

add_library(libcake STATIC ${cake_sources})
target_link_libraries(
  libcake 
  fmt::fmt 
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