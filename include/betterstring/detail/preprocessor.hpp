
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

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

#if defined(_MSVC_LANG)
    #if _MSVC_LANG >= 202002L
        #define BS_CXX20 1
    #else
        #define BS_CXX20 0
    #endif
#else
    #if __cplusplus >= 202002L
        #define BS_CXX20 1
    #else
        #define BS_CXX20 0
    #endif
#endif

#if BS_CXX20
    #define BS_CONSTEXPR_CXX20 constexpr
#else
    #define BS_CONSTEXPR_CXX20 inline
#endif

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
    #define BS_OS_WINDOWS 1
#else
    #define BS_OS_WINDOWS 0
#endif

#ifdef __has_builtin
    #define BS_HAS_BUILTIN(x) __has_builtin(x)
#else
    #define BS_HAS_BUILTIN(x) 0
#endif

#ifdef __has_cpp_attribute
    #define BS_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
    #define BS_HAS_CPP_ATTRIBUTE(x) 0
#endif

#if BS_HAS_BUILTIN(__builtin_debugtrap)
    #define BS_DEBUG_BREAK() __builtin_debugtrap()
#elif BS_COMP_GCC
    #define BS_DEBUG_BREAK() __builtin_trap()
#elif BS_COMP_MSVC
    #define BS_DEBUG_BREAK() __debugbreak()
#else
    #include <csignal>
    #if defined(SIGTRAP)
        #define BS_DEBUG_BREAK() std::raise(SIGTRAP)
    #else
        #define BS_DEBUG_BREAK() std::raise(SIGABRT)
    #endif
#endif

#if __has_include(<libassert/assert.hpp>)
    #define BS_HAS_LIBASSERT 1
#else
    #define BS_HAS_LIBASSERT 0
#endif

#if !defined(BS_VERIFY_FAIL) && !BS_HAS_LIBASSERT
    #include <cstdio>
    #define BS_VERIFY_FAIL(expression, message) \
        ((void)::std::fprintf(stderr, "%s:%d: assertion '%s' failed: %s\n", __FILE__, __LINE__, #expression, message), (void)BS_DEBUG_BREAK())
#endif

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

#if BS_COMP_MSVC
    #define BS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
    #define BS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#if (BS_HAS_BUILTIN(__builtin_operator_new) >= 201802L) && (BS_HAS_BUILTIN(__builtin_operator_delete) >= 201802L)
    #define BS_BUILTIN_OPERATOR_NEW __builtin_operator_new
    #define BS_BUILTIN_OPERATOR_DELETE __builtin_operator_delete
#endif

#ifndef BS_LIFETIMEBOUND
    #if BS_HAS_CPP_ATTRIBUTE(clang::lifetimebound)
        #define BS_LIFETIMEBOUND [[clang::lifetimebound]]
    #elif BS_HAS_CPP_ATTRIBUTE(msvc::lifetimebound)
        #define BS_LIFETIMEBOUND [[msvc::lifetimebound]]
    #else
        #define BS_LIFETIMEBOUND
    #endif
#endif

#if BS_HAS_BUILTIN(__builtin_constant_p)
    #define BS_BUILTIN_CONSTANT_P(expr) __builtin_constant_p(expr)
#else
    #define BS_BUILTIN_CONSTANT_P(expr) (false)
#endif

#ifndef NDEBUG
    #if BS_HAS_LIBASSERT
        #include <libassert/assert.hpp>
        #define BS_VERIFY(expression, message) DEBUG_ASSERT(expression, message)
    #else
        #if BS_COMP_CLANG || BS_COMP_GCC
            #define BS_VERIFY(expression, message) \
                (__builtin_expect((expression), 1) ? (void)0 : (void)(BS_VERIFY_FAIL(expression, message)))
        #else
            #define BS_VERIFY(expression, message) \
                ((expression) ? (void)0 : BS_VERIFY_FAIL(expression, message))
        #endif
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


