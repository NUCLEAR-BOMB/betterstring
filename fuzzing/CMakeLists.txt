
include("${PROJECT_SOURCE_DIR}/cmake/fuzzing.cmake")

add_executable(functions-fuzzing functions.cpp)
target_link_libraries(functions-fuzzing PRIVATE betterstring)
target_add_fuzzer(functions-fuzzing)

include(ProcessorCount)
ProcessorCount(cores_number)

add_custom_target(fuzzing
    COMMAND ${CMAKE_COMMAND}
        -D executable=$<TARGET_FILE:functions-fuzzing>
        -D forks=${cores_number}
        -D runs=1000000
        -P "${PROJECT_SOURCE_DIR}/fuzzing/run_fuzzing.cmake"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR} VERBATIM
)
add_dependencies(fuzzing functions-fuzzing)

