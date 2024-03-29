cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(betterstring VERSION 0.1 LANGUAGES CXX ASM_MASM)

option(BUILD_TEST "Build the betterstring-test target" ${PROJECT_IS_TOP_LEVEL})
option(USE_CLANG_TIDY "Enable the clang-tidy analyzer" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_BENCHMARK "Build the betterstring-benchmark target" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_FUZZ "Build fuzzer targets" ${PROJECT_IS_TOP_LEVEL})
option(USE_SANITIZERS "Enable sanitizers for entire project" ${PROJECT_IS_TOP_LEVEL})
option(USE_NATVIS "Enable .natvis file for visual studio debugger" TRUE)
option(USE_NATSTEPFILTER "Enable .natstepfilter file for visual studio debugger" TRUE)
option(PRINT_STACKTRACE "Print stacktrace if internal check is failed for betterstring-test target" FALSE)
# option(INSTALL_BETTERSTRING "" ${PROJECT_IS_TOP_LEVEL})

if (PROJECT_IS_TOP_LEVEL)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# use the masm64 assembler for compiling assembly language files
set(CMAKE_ASM_MASM_COMPILER "ml64.exe")

include("${PROJECT_SOURCE_DIR}/cmake/compiler_warnings.cmake")

set(main_headers
    "include/betterstring/functions.hpp"
    "include/betterstring/string_view.hpp"
    "include/betterstring/splited_string.hpp"
    "include/betterstring/char_traits.hpp"
    "include/betterstring/ascii.hpp"
    "include/betterstring/parsing.hpp"
    "include/betterstring/type_traits.hpp"
    "include/betterstring/string.hpp"
    "include/betterstring/safe_functions.hpp"
    "include/betterstring/find_result.hpp"
    "include/betterstring/allocators.hpp"
)
set(detail_headers
    "include/betterstring/detail/preprocessor.hpp"
    "include/betterstring/detail/integer_cmps.hpp"
    "include/betterstring/detail/parse_unsigned.hpp"
    "include/betterstring/detail/cpu_features.hpp"
    "include/betterstring/detail/reference_wrapper.hpp"
)
set(asm_src
    "src/strrfind_char_avx2.asm"
    "src/strcount_char_avx2.asm"
    "src/strlen_avx2.asm"
    "src/strfindn_char_avx2.asm"
    "src/strfirstof_avx2.asm"
)
set(strfirstof_files
    "src/strfirstof/cmp_1.asm"
    "src/strfirstof/cmp_2.asm"
    "src/strfirstof/cmp_3.asm"
    "src/strfirstof/cmp_4.asm"
    "src/strfirstof/cmp_5.asm"
    "src/strfirstof/cmp_6.asm"
    "src/strfirstof/cmp_any.asm"
)
add_library(betterstring
    ${main_headers}
    ${detail_headers}
    ${asm_src}
    ${strfirstof_files}
)
set_source_files_properties(${strfirstof_files} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Source Files/strfirstof" FILES ${strfirstof_files})

source_group("Header Files/detail" FILES ${detail_headers})
source_group("Source Files" FILES ${asm_src})

target_compile_options(betterstring PRIVATE $<$<COMPILE_LANGUAGE:ASM_MASM>:/Cp>)

# ignore warnings for betterstring library headers
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

if (USE_NATVIS)
    target_sources(betterstring PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/debugger/betterstring.natvis>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_DATADIR}/debugger/betterstring.natvis>"
    )
endif()
if (USE_NATSTEPFILTER)
    target_sources(betterstring PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/debugger/betterstring.natstepfilter>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_DATADIR}/debugger/betterstring.natstepfilter>"
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

#[[
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
]]
