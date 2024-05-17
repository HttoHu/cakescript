# fetch google test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
# add_compile_definitions(DISABLE_UNIT)
add_executable(unit
  ./utils.cpp
  ./lexer_test.cpp
  ./parser/expr_test.cpp
  ./parser/decl_test.cpp
  ./parser/loop_branch_test.cpp
  ./runtime/func_test.cpp
  ./runtime/array_test.cpp
  ./runtime/object_test.cpp
)
target_link_libraries(unit 
  GTest::gtest_main
  libcake
)
include(GoogleTest)
gtest_discover_tests(unit)
