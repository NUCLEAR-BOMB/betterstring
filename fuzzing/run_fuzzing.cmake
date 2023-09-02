
message(STATUS "Running fuzzing")

execute_process(
    COMMAND ${executable} -fork=${forks} -runs=${runs}
    RESULTS_VARIABLE result
)
if (NOT result STREQUAL 0)
    message(FATAL_ERROR "Fuzzing failed")
endif()
