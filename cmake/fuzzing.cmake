include_guard(GLOBAL)

function(target_add_fuzzer target)
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            target_compile_options(${target} PRIVATE /fsanitize=address /fsanitize=fuzzer)
            target_link_options(${target} PRIVATE /INCREMENTAL:NO)
            set_target_properties(${target} PROPERTIES
                MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>"
            )
            return()
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(${target} PRIVATE -g -fsanitize=fuzzer,address,undefined)
            target_link_options(${target} PRIVATE -fsanitize=fuzzer,address,undefined)
            set_target_properties(${target} ${libraries} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
            return()
        endif()
    endif()
    target_compile_options(${target} PRIVATE -g -fsanitize=fuzzer,address,undefined)
    target_link_options(${target} PRIVATE -fsanitize=fuzzer,address,undefined)
endfunction()
