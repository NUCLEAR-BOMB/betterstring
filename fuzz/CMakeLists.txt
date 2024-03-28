
include("${PROJECT_SOURCE_DIR}/cmake/sanitizers.cmake")

set(fuzz_targets "")

macro(add_fuzzer name)
    add_executable(${name}-fuzz ${ARGN})
    target_link_libraries(${name}-fuzz PRIVATE betterstring)
    target_add_sanitizer(${name}-fuzz SANITIZERS Address Fuzzer Undefined OPTIONS -fno-sanitize-recover=all)
    list(APPEND fuzz_targets "${name}-fuzz")
endmacro()

add_fuzzer(strrfind_ch strrfind_ch.cpp)
add_fuzzer(strrfind_str strrfind_str.cpp)
add_fuzzer(strcount_ch strcount_ch.cpp)
add_fuzzer(parse parse.cpp)
add_fuzzer(strlen strlen.cpp)
add_fuzzer(strfindn_ch strfindn_ch.cpp)
add_fuzzer(strfirstof strfirstof.cpp)

set_target_properties(${fuzz_targets} PROPERTIES FOLDER "fuzzers/")

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
