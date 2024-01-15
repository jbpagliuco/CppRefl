project("CppRefl")

# C++17 is the minimum supported language version.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(CppRefl STATIC "")

target_include_directories(CppRefl PUBLIC "${CMAKE_CURRENT_LIST_DIR}/Source")

add_subdirectory("Source")