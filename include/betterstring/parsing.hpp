
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <exception>

#include <betterstring/detail/parse_unsigned.hpp>
#include <betterstring/detail/result_with_sentinel.hpp>

namespace bs {

template<class T>
using parse_result = bs::detail::result_with_sentinel<T, parse_error, parse_error{}>;

template<class T, class Ch>
BS_FORCEINLINE
constexpr parse_result<T> parse(const Ch* const str, const std::size_t count) {
    if constexpr (std::is_unsigned_v<T>) {
        T result{};
        parse_error err{};
        if (detail::is_constant_evaluated()) {
            err = bs::detail::constexpr_parse_unsigned<T>(result, str, count);
        } else {
            err = bs::detail::parse_unsigned<T>(result, str, count);
        }
        return bs::parse_result<T>{result, err};
    } else {
        static_assert(sizeof(T) == 0, "unimplemented");
    }
}

}
