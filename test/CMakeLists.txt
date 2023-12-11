
include("${PROJECT_SOURCE_DIR}/cmake/compiler_warnings.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/enable_avx2.cmake")

add_executable(betterstring-test
    main.cpp string_view.cpp functions.cpp tools.hpp char_traits.cpp
)
target_add_warnings(betterstring-test)
target_link_libraries(betterstring-test PRIVATE betterstring)
target_compile_definitions(betterstring-test PRIVATE BS_DONT_INCLUDE_STRING)
target_enable_avx2(betterstring-test)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
    URL_HASH SHA256=ffa17fbc5953900994e2deec164bb8949879ea09b411e07f215bfbb1f87f4632
    FIND_PACKAGE_ARGS NAMES GTest
)
set(gtest_force_shared_crt TRUE CACHE BOOL "" FORCE)
set(BUILD_GMOCK FALSE)
set(INSTALL_GTEST FALSE)
set(CMAKE_DISABLE_FIND_PACKAGE_Python TRUE)
set(gtest_disable_pthreads TRUE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(betterstring-test PRIVATE GTest::gtest)
if (USE_SANITIZERS)
    include("${PROJECT_SOURCE_DIR}/cmake/sanitizers.cmake")
    target_add_sanitizer(betterstring-test gtest)
endif()

include(GoogleTest)
gtest_discover_tests(betterstring-test)

if (USE_CLANG_TIDY)
    include("${PROJECT_SOURCE_DIR}/cmake/clang_tidy.cmake")
    set_target_properties(betterstring-test PROPERTIES EXPORT_COMPILE_COMMANDS TRUE)
    add_clang_tidy_target(NAME tidy
        CONFIG "${PROJECT_SOURCE_DIR}/.clang-tidy"
        USE_COLOR QUIET SHOW_TIME
    )
endif()
