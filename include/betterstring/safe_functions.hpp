
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <betterstring/string_view.hpp>
#include <betterstring/functions.hpp>

namespace bs::safe {

namespace detail {
    template<class T>
    constexpr bool is_overlap(const T* first, std::size_t first_len, const T* second, std::size_t second_len) noexcept {
        return (first < second + second_len) && (second < first + first_len);
    }
}

enum class errorc {
    null_pointer      = 1,
    dest_null_pointer = 2,
    src_null_pointer  = 3,
    not_enough_space  = 4,
    overlapping        = 5,
};

class error {
    errorc error_code;
public:
    constexpr error(const errorc err) noexcept : error_code{err} {}

    constexpr errorc code() const noexcept { return error_code; }

    constexpr bs::string_view error_message() const noexcept {
        BS_VERIFY(error_code != errorc{}, "no error code");

        switch (error_code) {
        case errorc::null_pointer: return "pointer operand is null pointer with non-zero size";
        case errorc::dest_null_pointer: return "destination operand is null pointer with non-zero size";
        case errorc::src_null_pointer: return "source operand is null pointer with non-zero size";
        case errorc::not_enough_space: return "source buffer length is exceeding destination buffer";
        case errorc::overlapping: return "two memory buffers are overlapping";
        }
        BS_UNREACHABLE();
    }

    explicit constexpr operator bool() const noexcept {
        return error_code != errorc{};
    }

    friend constexpr bool operator==(const error left, const errorc right) noexcept {
        return left.error_code == right;
    }
    friend constexpr bool operator==(const errorc left, const error right) noexcept {
        return left == right.error_code;
    }
    friend constexpr bool operator!=(const error left, const errorc right) noexcept {
        return left.error_code != right;
    }
    friend constexpr bool operator!=(const errorc left, const error right) noexcept {
        return left != right.error_code;
    }
};

template<class T>
constexpr error strcopy(T* const dest, const std::size_t dest_len, const T* const src, const std::size_t count) noexcept {
    if (dest == nullptr && dest_len != 0) { return errorc::dest_null_pointer; }
    if (src == nullptr && count != 0) { return errorc::src_null_pointer; }
    if (count > dest_len) { return errorc::not_enough_space; }

    if (dest == nullptr || src == nullptr) { return errorc{}; }

    if (detail::is_overlap(dest, dest_len, src, count)) {
        return errorc::overlapping;
    }

    bs::strcopy(dest, src, count);

    return errorc{};
}

template<class T>
constexpr error strmove(T* const dest, const std::size_t dest_len, const T* const src, const std::size_t count) noexcept {
    if (dest == nullptr && dest_len != 0) { return errorc::dest_null_pointer; }
    if (src == nullptr && count != 0) { return errorc::src_null_pointer; }
    if (count > dest_len) { return errorc::not_enough_space; }

    if (dest == nullptr || src == nullptr) { return errorc{}; }

    bs::strmove(dest, src, count);

    return errorc{};
}

}
