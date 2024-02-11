
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

add_executable(betterstring-benchmark
    "main.cpp"
    "benchmarks.hpp"
    "html_boxplot.hpp"
)
target_link_libraries(betterstring-benchmark PRIVATE fmt::fmt nanobench betterstring)

