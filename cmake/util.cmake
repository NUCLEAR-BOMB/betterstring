include_guard(GLOBAL)

function(get_target_name out)
    set(out_tmp "")
    foreach(target IN LISTS ARGN)
        get_target_property(target_unaliased ${target} ALIASED_TARGET)
        if (target_unaliased STREQUAL "target_unaliased-NOTFOUND")
            list(APPEND out_tmp ${target})
        elseif (target_unaliased)
            list(APPEND out_tmp ${target_unaliased})
        else()
            list(APPEND out_tmp ${target})
        endif()
    endforeach()
    set(${out} ${out_tmp} PARENT_SCOPE)
endfunction()
