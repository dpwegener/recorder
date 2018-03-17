cmake_minimum_required(VERSION 3.0.0)
project(recorder VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_CLANG_TIDY clang-tidy;-warnings-as-errors=*;-header-filter=.*;-checks=-*,clang-analyzer-*,cppcoreguidelines-*)

include(CTest)
enable_testing()

add_executable(recorder main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
