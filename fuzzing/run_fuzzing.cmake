
message(STATUS "Running '${executable}' fuzzing")

execute_process(
    COMMAND ${executable} -fork=${forks} -max_total_time=${time} -print_final_stats=1 -print_pcs=1
    RESULTS_VARIABLE result
)
if (NOT result STREQUAL 0)
    message(FATAL_ERROR "Fuzzing '${executable}' failed with code ${result}")
endif()
