
include("${PROJECT_SOURCE_DIR}/cmake/sanitizers.cmake")

set(fuzz_targets "")

macro(add_fuzzer target_name)
    add_executable(${target_name} ${ARGN})
    target_link_libraries(${target_name} PRIVATE betterstring)
    target_add_sanitizer(${target_name} SANITIZERS Fuzzer Address)
    list(APPEND fuzz_targets ${target_name})
endmacro()

add_fuzzer(strrfind_ch_fuzz strrfind_ch.cpp)
add_fuzzer(strrfind_str_fuzz strrfind_str.cpp)
add_fuzzer(strcount_ch_fuzz strcount_ch.cpp)
add_fuzzer(parse_fuzz parse.cpp)
add_fuzzer(strlen_fuzz strlen.cpp)
add_fuzzer(strfindn_ch_fuzz strfindn_ch.cpp)
add_fuzzer(strfirst_of_fuzz strfirst_of.cpp)

include(ProcessorCount)
ProcessorCount(cores_number)

set(cmake_echo "${CMAKE_COMMAND}" -E echo)

set(fuzz_commands "")
foreach(fuzz_target IN LISTS fuzz_targets)
    list(APPEND fuzz_commands
        COMMAND ${cmake_echo} "| Launching target: '$<TARGET_NAME:${fuzz_target}>', executable: '$<TARGET_FILE:${fuzz_target}>' |"
        COMMAND "$<TARGET_FILE:${fuzz_target}>" -runs=$<IF:$<CONFIG:Release>,4000000,900000> -fork=${cores_number}
    )
endforeach()

add_custom_target(run-betterstring-fuzz
    COMMAND ${cmake_echo} "| Starting fuzzers using '${cores_number}' cores number |"
    ${fuzz_commands}
    COMMAND ${cmake_echo} "| Stopping fuzzers |"
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
    VERBATIM
)
