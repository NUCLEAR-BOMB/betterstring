
include("${PROJECT_SOURCE_DIR}/cmake/fuzzing.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/enable_avx2.cmake")

set(fuzz_targets "")

macro(add_fuzzer target_name)
    add_executable(${target_name} EXCLUDE_FROM_ALL ${ARGN})
    target_link_libraries(${target_name} PRIVATE betterstring)
    target_add_fuzzer(${target_name})
    target_enable_avx2(${target_name})
    list(APPEND fuzz_targets ${target_name})
endmacro()

add_fuzzer(strrfind_fuzz strrfind.cpp)

include(ProcessorCount)
ProcessorCount(cores_number)

set(fuzz_commands "")
foreach(fuzz_target IN LISTS fuzz_targets)
    list(APPEND fuzz_commands COMMAND ${CMAKE_COMMAND}
        -D executable=$<TARGET_FILE:${fuzz_target}>
        -D forks=${cores_number}
        #-D runs=1000000
        -D time=15
        -P "${PROJECT_SOURCE_DIR}/fuzzing/run_fuzzing.cmake"
    )
endforeach()

add_custom_target(betterstring-fuzz
    ${fuzz_commands}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR} VERBATIM
    SOURCES "run_fuzzing.cmake"
)
add_dependencies(betterstring-fuzz ${fuzz_targets})
