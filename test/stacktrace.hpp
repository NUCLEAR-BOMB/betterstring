#pragma once

#if BETTERSTRING_TEST_ENABLE_STACKTRACE
    #include <cpptrace/cpptrace.hpp>
    #include <cstdio>
    #define BS_ABORT(expression, message) \
        (::std::fprintf(stderr, "Assertion '%s' failed: %s\n\n", #expression, message), \
        ::cpptrace::generate_trace().print(), \
        BS_DEBUG_BREAK)
#endif
