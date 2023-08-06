cmake_minimum_required(VERSION 3.0.0)
add_library(tokenize STATIC
  primitive.cpp
  tokens.cpp
)

#
add_executable(tokenize_main main.cpp)
target_link_libraries(tokenize_main tokenize)

# parsers test
add_executable(tokenize_test
  parsers_test.cpp primitive_test.cpp mappers_test.cpp repeats_test.cpp either_test.cpp combinators_test.cpp
  tokens_test.cpp
)
target_link_libraries(tokenize_test tokenize gtest gtest_main pthread)
add_test(NAME tokenize_test COMMAND tokenize_test)
