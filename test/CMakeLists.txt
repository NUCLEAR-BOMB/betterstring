
include("${PROJECT_SOURCE_DIR}/cmake/compiler_warnings.cmake")

add_executable(betterstring-test
    string_view.cpp functions.cpp tools.hpp char_traits.cpp
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

if (USE_SANITIZERS)
    include("${PROJECT_SOURCE_DIR}/cmake/sanitizers.cmake")
    target_add_sanitizer(betterstring-test SANITIZERS Address UndefinedBehavior)
endif()

if (USE_CLANG_TIDY)
    include("${PROJECT_SOURCE_DIR}/cmake/clang_tidy.cmake")
    set_target_properties(betterstring-test PROPERTIES EXPORT_COMPILE_COMMANDS TRUE)
    add_clang_tidy_target(NAME tidy
        CONFIG "${PROJECT_SOURCE_DIR}/.clang-tidy"
        USE_COLOR QUIET SHOW_TIME
    )
endif()
