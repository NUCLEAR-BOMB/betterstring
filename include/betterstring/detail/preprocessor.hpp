#pragma once

#ifndef BS_DONT_INCLUDE_STRING
    #include <string>
#endif

#if __has_include(<version>)
    #include <version>
#endif

#ifdef __cpp_lib_char8_t
    #define BS_HAS_CHAR8_T 1
#else
    #define BS_HAS_CHAR8_T 0
#endif

#if defined(__clang__)
    #define BS_COMP_CLANG 1
    #define BS_COMP_GCC   0
    #define BS_COMP_MSVC  0
#elif defined(__GNUC__) || defined(__GNUG__)
    #define BS_COMP_CLANG 0
    #define BS_COMP_GCC   1
    #define BS_COMP_MSVC  0
#elif defined(_MSC_VER)
    #define BS_COMP_CLANG 0
    #define BS_COMP_GCC   0
    #define BS_COMP_MSVC  1
#else
    #define BS_COMP_CLANG 0
    #define BS_COMP_GCC   0
    #define BS_COMP_MSVC  0
#endif

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
    #define BS_OS_WINDOWS 1
#else
    #define BS_OS_WINDOWS 0
#endif

#if !BS_COMP_MSVC
    #define BS_IS_COMPILER_SUPPORTS_DYNAMIC_DISPATCH 1
#else
    #define BS_IS_COMPILER_SUPPORTS_DYNAMIC_DISPATCH 0
#endif

#ifdef __has_builtin
    #define BS_HAS_BUILTIN(x) __has_builtin(x)
#else
    #define BS_HAS_BUILTIN(x) 0
#endif

#if BS_HAS_BUILTIN(__builtin_debugtrap)
    #define BS_DEBUG_BREAK __builtin_debugtrap()
#elif BS_COMP_GCC
    #define BS_DEBUG_BREAK __builtin_trap()
#elif BS_COMP_MSVC
    #define BS_DEBUG_BREAK __debugbreak()
#else
    #include <csignal>
    #if defined(SIGTRAP)
        #define BS_DEBUG_BREAK std::raise(SIGTRAP)
    #else
        #define BS_DEBUG_BREAK std::raise(SIGABRT)
    #endif
#endif

#define BS_ABORT(expression, message) \
    ((void)std::fprintf(stderr, "%s:%d: assertion '%s' failed: %s\n", __FILE__, __LINE__, #expression, message), (void)BS_DEBUG_BREAK)

#if BS_HAS_BUILTIN(__builtin_assume)
    #define BS_ASSUME(expression) __builtin_assume(expression)
#elif BS_COMP_GCC
    #define BS_ASSUME(expression) \
        ((expression) ? (void)0 : (void)__builtin_unreachable())
#elif BS_COMP_MSVC
    #define BS_ASSUME(expression) __assume(expression)
#else
    #define BS_ASSUME(expression) ((void)0)
#endif

#if BS_COMP_CLANG || BS_COMP_GCC
    #define BS_CONST_FN [[gnu::const]]
#elif defined(_MSC_VER)
    #define BS_CONST_FN __declspec(noalias)
#else
    #define BS_CONST_FN
#endif

#if BS_COMP_CLANG || BS_COMP_GCC
    #define BS_UNREACHABLE() __builtin_unreachable()
#elif BS_COMP_MSVC
    #define BS_UNREACHABLE() __assume(false)
#else
    #define BS_UNREACHABLE()
#endif

#if BS_COMP_CLANG || BS_COMP_GCC
    #define BS_FLATTEN [[gnu::flatten]]
#elif BS_COMP_MSVC
    #define BS_FLATTEN [[msvc::flatten]]
#else
    #define BS_FLATTEN
#endif

#if BS_COMP_CLANG || BS_COMP_GCC || BS_COMP_MSVC
    #define BS_RESTRICT __restrict
#else
    #define BS_RESTRICT
#endif

#if BS_COMP_CLANG
    #define BS_FORCEINLINE [[clang::always_inline]]
#elif BS_COMP_GCC
    #define BS_FORCEINLINE [[gnu::always_inline]]
#elif BS_COMP_MSVC
    #define BS_FORCEINLINE [[msvc::forceinline]]
#else
    #define BS_FORCEINLINE
#endif

#if BS_COMP_CLANG
    #define BS_NOINLINE [[clang::noinline]]
#elif BS_COMP_GCC
    #define BS_NOINLINE [[gnu::noinline]]
#elif BS_COMP_MSVC
    #define BS_NOINLINE [[msvc::noinline]]
#else
    #define BS_NOINLINE
#endif

#ifndef NDEBUG
    #if BS_COMP_CLANG || BS_COMP_GCC
        #define BS_VERIFY(expression, message) \
            (__builtin_expect((expression), 1) ? (void)0 : BS_ABORT(expression, message))
    #else
        #define BS_VERIFY(expression, message) \
            ((expression) ? (void)0 : BS_ABORT(expression, message))
    #endif
#else
    #define BS_VERIFY(expression, message) BS_ASSUME(expression)
#endif

#ifdef __AVX2__
    #define BS_ARCH_AVX2 1
#else
    #define BS_ARCH_AVX2 0
#endif

#define BS_ARCH_NO_AVX2 0


