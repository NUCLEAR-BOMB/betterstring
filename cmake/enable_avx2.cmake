include_guard(GLOBAL)

function(target_enable_avx2 target)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE /arch:AVX2)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${target} PRIVATE -mavx2)
    endif()
endfunction()
