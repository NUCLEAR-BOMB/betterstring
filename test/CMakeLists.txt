
include("${PROJECT_SOURCE_DIR}/cmake/compiler_warnings.cmake")

add_executable(betterstring-test
    "string_view.cpp"
    "functions.cpp"
    "safe_functions.cpp"
    "char_traits.cpp"
    "ascii.cpp"
    "parsing.cpp"
    "string.cpp"
    "cpu_dispatch.cpp"
    "allocators.cpp"

    "util.hpp"
)
target_add_warnings(betterstring-test)
target_link_libraries(betterstring-test PRIVATE betterstring)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.0
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS 3.5.0
)
FetchContent_MakeAvailable(Catch2)

target_compile_features(Catch2 PRIVATE cxx_std_17)
target_link_libraries(betterstring-test PRIVATE Catch2::Catch2WithMain)
set_target_properties(Catch2 PROPERTIES
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Catch2,INTERFACE_INCLUDE_DIRECTORIES>
)
set_target_properties(Catch2 Catch2WithMain PROPERTIES FOLDER "external/")

target_compile_definitions(betterstring-test PRIVATE CATCH_CONFIG_FAST_COMPILE)

if (USE_SANITIZERS)
    include("${PROJECT_SOURCE_DIR}/cmake/sanitizers.cmake")
    target_add_sanitizer(betterstring-test SANITIZERS Address Undefined LIBRARIES Catch2 Catch2WithMain)
endif()

if (USE_CLANG_TIDY)
    include("${PROJECT_SOURCE_DIR}/cmake/clang_tidy.cmake")
    set_target_properties(betterstring-test PROPERTIES EXPORT_COMPILE_COMMANDS TRUE)
    add_clang_tidy_target(NAME clang-tidy
        CONFIG "${PROJECT_SOURCE_DIR}/.clang-tidy"
        EXTRA_ARG "-std=c++17" "-Wno-unknown-warning-option"
        USE_COLOR QUIET SHOW_TIME
    )
endif()

set_target_properties(betterstring-test PROPERTIES
    VS_DEBUGGER_ENVIRONMENT "UBSAN_OPTIONS=print_stacktrace=1"
)
add_custom_target(run-betterstring-test VERBATIM
    COMMAND "${CMAKE_COMMAND}" -E env "UBSAN_OPTIONS=print_stacktrace=1" "$<TARGET_FILE:betterstring-test>"
)
# dependency will be added automatically
