cmake_minimum_required(VERSION 3.0.0)
project(wedge VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(tokenizes)
add_executable(wedge main.cpp)
target_link_libraries(wedge tokenize)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
