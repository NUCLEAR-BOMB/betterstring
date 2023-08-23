
#include("${PROJECT_SOURCE_DIR}/cmake/compiler_warnings.cmake")

add_executable(betterstring-benchmark string_view.cpp fixture.hpp)
target_add_warnings(betterstring-benchmark)
target_link_libraries(betterstring-benchmark PRIVATE betterstring)

include(FetchContent)
FetchContent_Declare(
    googlebenchmark
    URL https://github.com/google/benchmark/archive/refs/tags/v1.8.2.zip
    URL_HASH SHA256=0094b77c01e9973876f1831f76cbae43fdaf09b424e467dc4171abaa6884e455
    FIND_PACKAGE_ARGS NAMES benchmark
)

set(BENCHMARK_ENABLE_TESTING FALSE)
set(BENCHMARK_ENABLE_WERROR FALSE)
set(BENCHMARK_FORCE_WERROR FALSE)
FetchContent_MakeAvailable(googlebenchmark)

target_link_libraries(betterstring-benchmark PRIVATE benchmark::benchmark benchmark::benchmark_main)
