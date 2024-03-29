include_guard(GLOBAL)

include("${PROJECT_SOURCE_DIR}/cmake/util.cmake")

function(target_add_warnings target)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:
        /Wall /WX /permissive-
        /wd5027 # 'type': move assignment operator was implicitly defined as deleted
        /wd4626 # 'derived class': assignment operator was implicitly defined as deleted because a base class assignment operator is inaccessible or deleted
        /wd5026 # 'type': move constructor was implicitly defined as deleted
        /wd4625 # 'derived class': copy constructor was implicitly defined as deleted because a base class copy constructor is inaccessible or deleted
        /wd5264 # 'variable-name': 'const' variable is not used
        /wd5262 # implicit fall-through occurs here between cases
        /wd4820 # 'bytes' bytes padding added after construct 'member_name'
        /wd4514 # 'function' : unreferenced inline function has been removed
        /wd5246 # 'member': the initialization of a subobject should be wrapped in braces
        /wd5045 # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
        /wd4710 # 'function' : function not inlined
        /wd4711 # function 'function' selected for inline expansion
        /wd4866 # 'file(line_number)' compiler may not enforce left-to-right evaluation order for call to operator_name
        /wd5072 # ASAN enabled without debug information emission. Enable debug info for better ASAN error reporting
        /wd4623 # 'derived class' : default constructor was implicitly defined as deleted
        /wd4388 # 'token' : signed/unsigned mismatch
        /wd4868 # 'file(line_number)' compiler may not enforce left-to-right evaluation order in braced initializer list
        /wd5267 # definition of implicit copy constructor/assignment operator for 'type' is deprecated because it has a user-provided assignment operator/copy constructor
        /wd4574 # 'Identifier' is defined to be '0': did you mean to use '#if identifier'?
        /wd4365 # 'action' : conversion from 'type_1' to 'type_2', signed/unsigned mismatch
        >)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:
        -Wall # Enables all the warnings
        -Wextra # Enables some extra warning flags that are not enabled by -Wall
        -Wpedantic # Issue all the warnings demanded by strict ISO C and ISO C++
        -Wconversion # Warn for implicit conversions that may alter a value
        -Werror # Make all warnings into errors.
        -Wcast-align # Warn whenever a pointer is cast such that the required alignment of the target is increased
        -Wunused # -Wunused options combined
        -Wconversion # Warn for implicit conversions that may alter a value
        -Wdouble-promotion # Give a warning when a value of type float is implicitly promoted to double
        -Wold-style-cast # Warn for C style casting
        -Wshadow # Warn whenever a local variable or type declaration shadows another variable
        -Wno-sign-conversion # Don't warn for implicit conversions that may change the sign of an integer value
        -Wno-old-style-cast # Disable warning for using C-style casts
        -Wno-c++98-compat
        -Wno-pre-c++17-compat
        -Wno-pre-c++14-compat
        -Wno-unsafe-buffer-usage
        -Wno-ctad-maybe-unsupported
        -Wno-c++98-compat-bind-to-temporary-copy
        -Wno-global-constructors
        -Wno-c++98-compat-pedantic
        -Wno-zero-as-null-pointer-constant
        -Wno-shadow-field-in-constructor
        -Wno-self-assign-overloaded
        -Wno-assume
        >)
    endif()
endfunction()



function(target_suppress_warnings target)
    get_target_name(target_name ${target})
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        get_target_property(compile_options ${target_name} COMPILE_OPTIONS)
        list(REMOVE_ITEM compile_options "/W4" "/W3" "/W2" "/W1" "/W0" "/w4" "/w3" "/w2" "/w1" "/w0" "/w" "/Wall")
        list(APPEND compile_options "$<$<COMPILE_LANGUAGE:CXX>:/w>")
        set_target_properties(${target_name} PROPERTIES COMPILE_OPTIONS "${compile_options}")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target_name} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-w>)
    endif()
endfunction()
