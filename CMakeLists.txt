cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(betterstring LANGUAGES CXX)

option(BUILD_TEST "" TRUE)
option(USE_CLANG_TIDY "" TRUE)
option(BUILD_BENCHMARK "" TRUE)
option(BUILD_FUZZ "" TRUE)
option(USE_SANITIZERS "" TRUE)

add_library(betterstring INTERFACE "include/betterstring.hpp")
target_include_directories(betterstring INTERFACE "include/")
target_compile_features(betterstring INTERFACE cxx_std_17)

target_compile_options(betterstring INTERFACE $<$<CXX_COMPILER_ID:MSVC>:
	/Zc:__cplusplus /Zc:inline /Zc:preprocessor>
)

if (BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

if (BUILD_BENCHMARK)
    add_subdirectory(benchmark)
endif()

if (BUILD_FUZZ)
    add_subdirectory(fuzz)
endif()
