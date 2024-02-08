
include("${PROJECT_SOURCE_DIR}/cmake/sanitizers.cmake")

set(fuzz_targets "")

macro(add_fuzzer target_name)
    add_executable(${target_name} EXCLUDE_FROM_ALL ${ARGN})
    target_link_libraries(${target_name} PRIVATE betterstring)
    target_add_sanitizer(${target_name} SANITIZERS Fuzzer Address)
    list(APPEND fuzz_targets ${target_name})
endmacro()

add_fuzzer(strrfind_ch_fuzz strrfind_ch.cpp)
add_fuzzer(strrfind_str_fuzz strrfind_str.cpp)
add_fuzzer(strcount_ch_fuzz strcount_ch.cpp)
add_fuzzer(parse_fuzz parse.cpp)

include(ProcessorCount)
ProcessorCount(cores_number)

set(fuzz_commands "")
foreach(fuzz_target IN LISTS fuzz_targets)
    list(APPEND fuzz_commands COMMAND ${CMAKE_COMMAND}
        -D executable=$<TARGET_FILE:${fuzz_target}>
        -D forks=${cores_number}
        #-D runs=1000000
        -D time=5
        -P "${PROJECT_SOURCE_DIR}/fuzz/run_fuzzing.cmake"
    )
endforeach()

add_custom_target(betterstring-fuzz
    ${fuzz_commands}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR} VERBATIM
    SOURCES "run_fuzzing.cmake"
)
add_dependencies(betterstring-fuzz ${fuzz_targets})
