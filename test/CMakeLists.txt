
add_executable(betterstring-test main.cpp)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
    URL_HASH SHA256=ffa17fbc5953900994e2deec164bb8949879ea09b411e07f215bfbb1f87f4632
    FIND_PACKAGE_ARGS NAMES GTest
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK NO CACHE BOOL "" FORCE)
set(INSTALL_GTEST NO CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(betterstring-test PRIVATE betterstring GTest::gtest)