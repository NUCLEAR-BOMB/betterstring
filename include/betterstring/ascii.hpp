
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <betterstring/detail/preprocessor.hpp>
#include <type_traits>

// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4203.html

namespace bs {
namespace detail {
    template<class T>
    struct is_ascii_compatible_impl { static constexpr bool value = false; };
    template<> struct is_ascii_compatible_impl<char> { static constexpr bool value = true; };
    template<> struct is_ascii_compatible_impl<char16_t> { static constexpr bool value = true; };
    template<> struct is_ascii_compatible_impl<char32_t> { static constexpr bool value = true; };
#if BS_HAS_CHAR8_T
    template<> struct is_ascii_compatible_impl<char8_t> { static constexpr bool value = true; };
#endif

    template<class C>
    using enable_is_ascii_compatible = std::enable_if_t<is_ascii_compatible_impl<C>::value, int>;
}

template<class T>
inline constexpr bool is_ascii_compatible = detail::is_ascii_compatible_impl<T>::value;
}

namespace bs::ascii {

template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_digit(const Ch ch) noexcept {
    return ch >= '0' && ch <= '9';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_hexdigit(const Ch ch) noexcept {
    return is_digit(ch) || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f');
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_octdigit(const Ch ch) noexcept {
    return ch >= '0' && ch <= '7';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_lowercase(const Ch ch) noexcept {
    return ch >= 'a' && ch <= 'z';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_uppercase(const Ch ch) noexcept {
    return ch >= 'A' && ch <= 'Z';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_alphabetic(const Ch ch) noexcept {
    return is_lowercase(ch) || is_uppercase(ch);
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_alphanumeric(const Ch ch) noexcept {
    return is_alphabetic(ch) || is_digit(ch);
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_punctuation(const Ch ch) noexcept {
    return (ch >= '!' && ch <= '/') || (ch >= ':' && ch <= '@') || (ch >= '[' && ch <= '`') || (ch >= '{' && ch <= '~');
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_graphic(const Ch ch) noexcept {
    return ch >= '!' && ch <= '~';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_blank(const Ch ch) noexcept {
    return ch == '\t' || ch == ' ';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_whitespace(const Ch ch) noexcept {
    return (ch >= '\t' && ch <= '\r') || ch == ' ';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_printable(const Ch ch) noexcept {
    return ch >= ' ' && ch <= '~';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_control(const Ch ch) noexcept {
    return ch >= '\0' && ch <= '\37';
}

template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_ascii(const Ch ch) noexcept {
    return ch >= 0 && ch <= 127;
}

template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr Ch to_lowercase(const Ch ch) noexcept {
    return is_uppercase(ch) ? Ch(ch + ('a' - 'A')) : ch;
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr Ch to_uppercase(const Ch ch) noexcept {
    return is_lowercase(ch) ? Ch(ch - ('a' - 'A')) : ch;
}

template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr int to_digit(const Ch ch) noexcept {
    BS_VERIFY(is_digit(ch), "character must be a digit");
    return ch - '0';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr int to_hexdigit(const Ch ch) noexcept {
    BS_VERIFY(is_hexdigit(ch), "character must be a hexadecimal digit");
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return ch - '0';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr int to_octdigit(const Ch ch) noexcept {
    BS_VERIFY(is_octdigit(ch), "character must be a octal digit");
    return ch - '0';
}

template<class Ch = char, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr Ch from_digit(const int d) noexcept {
    BS_VERIFY(d >= 0 && d <= 9, "integer must be a single digit");
    return static_cast<Ch>(d + '0');
}

template<class Ch = char, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr Ch from_lowercase_hexdigit(const int d) noexcept {
    BS_VERIFY(d >= 0 && d <= 15, "integer must be a single hexadecimal digit");
    return static_cast<Ch>(d <= 9 ? d + '0' : d + 'a' - 10);
}
template<class Ch = char, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr Ch from_uppercase_hexdigit(const int d) noexcept {
    BS_VERIFY(d >= 0 && d <= 15, "integer must be a single hexadecimal digit");
    return static_cast<Ch>(d <= 9 ? d + '0' : d + 'A' - 10);
}

template<class Ch = char, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr Ch from_octdigit(const int d) noexcept {
    BS_VERIFY(d >= 0 && d <= 7, "integer must be a single octal digit");
    return static_cast<Ch>(d + '0');
}

template<class T>
constexpr int ci_strcomp(const T* const left, const T* const right, const std::size_t count) noexcept {
    static_assert(detail::is_ascii_compatible_impl<T>::value, "T must be ASCII compatible");

    constexpr int64_t delta = 'a' - 'A';

    int64_t a{}, b{}, c{};
    for (std::size_t i = 0; i < count; ++i) {
        a = int64_t(left[i]);
        b = a - delta >= 26 ? a : a + delta; // cmov

        c = int64_t(right[i]);
        a = c - delta >= 26 ? c : c + delta; // cmov

        if (b != a) {
            return int(b - a);
        }
    }
    return 0;
}

template<class T>
constexpr int ci_strcomp(const T* const left, const std::size_t left_len, const T* const right, const std::size_t right_len) noexcept {
    static_assert(detail::is_ascii_compatible_impl<T>::value, "T must be ASCII compatible");

    if (left_len < right_len) { return -1; }
    if (left_len > right_len) { return 1; }
    return ci_strcomp(left, right, left_len);
}

}
