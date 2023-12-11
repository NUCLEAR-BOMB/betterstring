
include(FetchContent)
FetchContent_Declare(
    nanobench
    GIT_REPOSITORY https://github.com/martinus/nanobench.git
    GIT_TAG v4.3.11
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nanobench)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.1.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(fmt)

add_executable(betterstring-benchmark
    "main.cpp"
    "samples.hpp"
)
target_link_libraries(betterstring-benchmark PRIVATE fmt::fmt nanobench betterstring)

