
include("${PROJECT_SOURCE_DIR}/cmake/compiler_warnings.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/clang_tidy.cmake")

add_executable(betterstring-test main.cpp string_view.cpp)
target_add_warnings(betterstring-test)
target_link_libraries(betterstring-test PRIVATE betterstring)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
    URL_HASH SHA256=ffa17fbc5953900994e2deec164bb8949879ea09b411e07f215bfbb1f87f4632
    FIND_PACKAGE_ARGS NAMES GTest
)

set(gtest_force_shared_crt TRUE CACHE BOOL "" FORCE)
set(BUILD_GMOCK FALSE CACHE BOOL "" FORCE)
set(INSTALL_GTEST FALSE CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(betterstring-test PRIVATE GTest::gtest)

include(GoogleTest)
gtest_discover_tests(betterstring-test)

set_target_properties(betterstring-test PROPERTIES EXPORT_COMPILE_COMMANDS TRUE)
add_clang_tidy_target(tidy)
