cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(betterstring VERSION 0.1 LANGUAGES CXX ASM_MASM)

option(BUILD_TEST "" ${PROJECT_IS_TOP_LEVEL})
option(USE_CLANG_TIDY "" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_BENCHMARK "" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_FUZZ "" ${PROJECT_IS_TOP_LEVEL})
option(USE_SANITIZERS "" ${PROJECT_IS_TOP_LEVEL})
option(INSTALL_BETTERSTRING "" ${PROJECT_IS_TOP_LEVEL})

include("${PROJECT_SOURCE_DIR}/cmake/compiler_warnings.cmake")

add_library(betterstring
    "include/betterstring/functions.hpp"
    "include/betterstring/string_view.hpp"
    "include/betterstring/splited_string.hpp"
    "include/betterstring/char_traits.hpp"
    "include/betterstring/ascii.hpp"
    "include/betterstring/parsing.hpp"
    "include/betterstring/type_traits.hpp"
    "include/betterstring/string.hpp"

    "include/betterstring/detail/preprocessor.hpp"
    "include/betterstring/detail/integer_cmps.hpp"
    "include/betterstring/detail/parse_unsigned.hpp"
    "include/betterstring/detail/cpu_features.hpp"

    "src/strrfind_char_avx2.asm"
)
set_target_properties(betterstring PROPERTIES LINKER_LANGUAGE CXX)

target_compile_options(betterstring PRIVATE $<$<COMPILE_LANGUAGE:ASM_MASM>:/Cp>)

target_add_warnings(betterstring)

if (NOT PROJECT_IS_TOP_LEVEL)
    set(include_as_system "SYSTEM")
endif()

target_include_directories(betterstring ${include_as_system} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(betterstring PUBLIC cxx_std_17)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(betterstring PUBLIC
        $<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus /Zc:inline /Zc:preprocessor>
        $<$<COMPILE_LANGUAGE:ASM_MASM>:/WX>
    )
endif()

if (BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

if (BUILD_BENCHMARK)
    add_subdirectory(benchmark)
endif()

if (BUILD_FUZZ)
    add_subdirectory(fuzz)
endif()

if (INSTALL_BETTERSTRING)
    include(CMakePackageConfigHelpers)
    include(GNUInstallDirs)

    # Targets
    install(
        TARGETS betterstring
        EXPORT "betterstringTargets"
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
    install(EXPORT betterstringTargets
        FILE betterstringTargets.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/betterstring"
    )
    # Version
    write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/betterstringConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
        ARCH_INDEPENDENT
    )
    # Config
    configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/betterstringConfig.cmake.in"
        "${PROJECT_BINARY_DIR}/betterstringConfig.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/betterstring"
    )
    # Install version and config
    install(FILES
        "${PROJECT_BINARY_DIR}/betterstringConfig.cmake"
        "${PROJECT_BINARY_DIR}/betterstringConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/betterstring"
    )
    # Install 'include/'
    install(
        DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )

endif()
