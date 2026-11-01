cmake_minimum_required(VERSION 3.14)
project(movie_library LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  add_compile_options(/W4 /utf-8)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(movie_library src/main.cpp)

enable_testing()
add_executable(binary_tree_test tests/binary_tree_test.cpp)
add_test(NAME binary_tree_test COMMAND binary_tree_test)
