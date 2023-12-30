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
constexpr bool is_hex_digit(const Ch ch) noexcept {
    return is_digit(ch) || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f');
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_lower(const Ch ch) noexcept {
    return ch >= 'a' && ch <= 'z';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_upper(const Ch ch) noexcept {
    return ch >= 'A' && ch <= 'Z';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_alpha(const Ch ch) noexcept {
    return is_lower(ch) || is_upper(ch);
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_alnum(const Ch ch) noexcept {
    return is_alpha(ch) || is_digit(ch);
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_punct(const Ch ch) noexcept {
    return (ch >= '!' && ch <= '/') || (ch >= ':' && ch <= '@') || (ch >= '[' && ch <= '`') || (ch >= '{' && ch <= '~');
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_graph(const Ch ch) noexcept {
    return ch >= '!' && ch <= '~';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_blank(const Ch ch) noexcept {
    return ch == '\t' || ch == ' ';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_space(const Ch ch) noexcept {
    return (ch >= '\t' && ch <= '\r') || ch == ' ';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_print(const Ch ch) noexcept {
    return ch >= ' ' && ch <= '~';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr bool is_ascii(const Ch ch) noexcept {
    return ch >= 0 && ch <= 127;
}

template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr Ch to_lower(const Ch ch) noexcept {
    return is_upper(ch) ? Ch(ch + ('a' - 'A')) : ch;
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr Ch to_upper(const Ch ch) noexcept {
    return is_lower(ch) ? Ch(ch - ('a' - 'A')) : ch;
}

template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr int to_digit(const Ch ch) noexcept {
    BS_VERIFY(is_digit(ch), "character must be a digit");
    return ch - '0';
}
template<class Ch, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr int to_hex_digit(const Ch ch) noexcept {
    BS_VERIFY(is_hex_digit(ch), "character must be a hexadecimal digit");
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return ch - '0';
}

template<class Ch = char, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr Ch from_digit(const int d) noexcept {
    BS_VERIFY(d >= 0 && d <= 9, "integer must be a single digit");
    return static_cast<Ch>(d + '0');
}

template<class Ch = char, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr Ch from_lower_hex_digit(const int d) noexcept {
    BS_VERIFY(d >= 0 && d <= 15, "integer must be a single hexadecimal digit");
    return static_cast<Ch>(d <= 9 ? d + '0' : d + 'a' - 10);
}
template<class Ch = char, detail::enable_is_ascii_compatible<Ch> = 0>
constexpr Ch from_upper_hex_digit(const int d) noexcept {
    BS_VERIFY(d >= 0 && d <= 15, "integer must be a single hexadecimal digit");
    return static_cast<Ch>(d <= 9 ? d + '0' : d + 'A' - 10);
}


}
