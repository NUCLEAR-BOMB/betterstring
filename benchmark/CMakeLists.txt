
include(FetchContent)
FetchContent_Declare(
    nanobench
    GIT_REPOSITORY https://github.com/martinus/nanobench.git
    GIT_TAG v4.3.11
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nanobench)

add_executable(betterstring-benchmark
    "main.cpp"
    "samples.hpp"
)
target_link_libraries(betterstring-benchmark PRIVATE nanobench betterstring)

