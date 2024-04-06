
include("${PROJECT_SOURCE_DIR}/cmake/compiler_warnings.cmake")

add_executable(betterstring-test
    "string_view.cpp"
    "functions.cpp"
    "safe_functions.cpp"
    "char_traits.cpp"
    "ascii.cpp"
    "parsing.cpp"
    "string.cpp"
    "allocators.cpp"
    "unicode.cpp"

    "main.cpp"

    "util.hpp"
)
target_add_warnings(betterstring-test)
target_link_libraries(betterstring-test PRIVATE betterstring)
set_target_properties(betterstring-test PROPERTIES UNITY_BUILD FALSE)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(betterstring-test PRIVATE
        /Zc:inline /RTCs /RTCu
    )
endif()

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
target_link_libraries(betterstring-test PRIVATE Catch2::Catch2)
set_target_properties(Catch2 PROPERTIES
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Catch2::Catch2,INTERFACE_INCLUDE_DIRECTORIES>
)

target_compile_definitions(betterstring-test PRIVATE CATCH_CONFIG_FAST_COMPILE)

if (PRINT_STACKTRACE)
    include(FetchContent)
    FetchContent_Declare(
        libassert
        GIT_REPOSITORY https://github.com/jeremy-rifkin/libassert.git
        GIT_TAG        v2.0.0
    )
    FetchContent_MakeAvailable(libassert)
    target_link_libraries(betterstring-test PRIVATE libassert::assert)
    target_suppress_warnings(libassert::assert)
    get_target_name(libassert_name libassert::assert)

    if (CMAKE_SYSTEM_NAME MATCHES "Windows")
        add_custom_command(TARGET betterstring-test POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different $<TARGET_FILE:libassert::assert> $<TARGET_FILE_DIR:betterstring-test>
        )
    endif()
    set(libassert_target libassert::assert)
endif()

set_target_properties(${libassert_name} Catch2 PROPERTIES FOLDER "external/")

if (USE_SANITIZERS)
    include("${PROJECT_SOURCE_DIR}/cmake/sanitizers.cmake")
    target_add_sanitizer(betterstring-test SANITIZERS Address Undefined
        LIBRARIES Catch2::Catch2 ${libassert_target}
    )
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
    VS_DEBUGGER_ENVIRONMENT "UBSAN_OPTIONS=print_stacktrace=1 ASAN_OPTIONS=detect_odr_violation=0"
)
add_custom_target(run-betterstring-test VERBATIM
    COMMAND "${CMAKE_COMMAND}" -E env "UBSAN_OPTIONS=print_stacktrace=1" "ASAN_OPTIONS=detect_odr_violation=0" "$<TARGET_FILE:betterstring-test>"
)
# dependency will be added automatically
