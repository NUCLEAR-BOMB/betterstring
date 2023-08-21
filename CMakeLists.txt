cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(betterstring LANGUAGES CXX)

add_library(betterstring INTERFACE "include/betterstring.hpp")
target_include_directories(betterstring INTERFACE "include/")
target_compile_features(betterstring INTERFACE cxx_std_17)

target_compile_options(betterstring INTERFACE $<$<CXX_COMPILER_ID:MSVC>:
	/Zc:__cplusplus /Zc:inline /Zc:preprocessor>
)

enable_testing()
add_subdirectory(test)
