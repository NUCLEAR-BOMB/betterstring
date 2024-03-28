
include(FetchContent)
FetchContent_Declare(
    nanobench
    GIT_REPOSITORY https://github.com/martinus/nanobench.git
    GIT_TAG v4.3.11
    GIT_SHALLOW TRUE
)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nanobench fmt)
set_target_properties(nanobench fmt PROPERTIES FOLDER "external/")
target_suppress_warnings(fmt)

add_executable(betterstring-benchmark
    "main.cpp"
    "html_boxplot.hpp"
    "add_benchmark_macro.hpp"
    "util.hpp"

    "benchmarks/parsing.hpp"
    "benchmarks/functions.hpp"
)
target_link_libraries(betterstring-benchmark PRIVATE fmt::fmt nanobench betterstring)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(betterstring-benchmark PRIVATE
        -Wno-c++20-extensions
    )
endif()

