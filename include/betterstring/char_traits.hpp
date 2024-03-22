
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <betterstring/functions.hpp>

namespace bs {

template<class T>
class char_traits
#ifndef BS_DONT_INCLUDE_STRING
    : private std::char_traits<T>
#endif
{
public:
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using char_type = T;
#ifndef BS_DONT_INCLUDE_STRING
private:
    using base = std::char_traits<T>;
public:
    using int_type = typename base::int_type;
    using off_type = typename base::off_type;
    using pos_type = typename base::pos_type;
    using state_type = typename base::state_type;

    using base::to_char_type;
    using base::to_int_type;
    using base::eq_int_type;
    using base::eof;
    using base::not_eof;
#endif

    static constexpr std::size_t string_container_alignment = 32;

    static constexpr void assign(char_type& dest, const char_type& src) noexcept {
        dest = src;
    }
    static constexpr void assign(char_type* const dest, const size_type count, const char_type ch) noexcept {
        bs::strfill(dest, count, ch);
    }
    static constexpr bool eq(const char_type l, const char_type r) noexcept {
        return l == r;
    }
    static constexpr bool lt(const char_type l, const char_type r) noexcept {
        return l < r;
    }
    static constexpr char_type* move(char_type* const dest, const char_type* const src, const std::size_t count) noexcept {
        bs::strmove(dest, src, count);
        return dest;
    }
    static constexpr char_type* copy(char_type* const dest, const char_type* const src, const std::size_t count) noexcept {
        bs::strcopy(dest, src, count);
        return dest;
    }
    static constexpr int compare(const char_type* const left, const char_type* const right, const std::size_t count) noexcept {
        return bs::strcomp(left, right, count);
    }
    static constexpr std::size_t length(const char_type* const str) noexcept {
        return bs::strlen(str);
    }
    static constexpr const char_type* find(const char_type* const str, const std::size_t count, const char_type& ch) noexcept {
        return bs::strfind(str, count, ch);
    }
    static constexpr const char_type* find_not(const char_type* const str, const std::size_t count, const char_type& ch) noexcept {
        return bs::strfindn(str, count, ch);
    }
    static constexpr const char_type* findstr(const char_type* const str, const std::size_t count, const char_type* const substr, const std::size_t substr_len) noexcept {
        return bs::strfind(str, count, substr, substr_len);
    }
    static constexpr const char_type* rfind(const char_type* const str, const std::size_t count, const char_type ch) noexcept {
        return bs::strrfind(str, count, ch);
    }
    static constexpr const char_type* rfindstr(const char_type* const str, const std::size_t count, const char_type* const substr, const std::size_t substr_len) noexcept {
        return bs::strrfind(str, count, substr, substr_len);
    }
    static constexpr const char_type* first_of(const char_type* const str, const std::size_t count, const char_type* const needle, const std::size_t needle_len) {
        return bs::strfirstof(str, count, needle, needle_len);
    }
    static constexpr const char_type* first_not_of(const char_type* const str, const std::size_t count, const char_type* const needle, const std::size_t needle_len) {
        return bs::strfirstnof(str, count, needle, needle_len);
    }

    static constexpr size_type count(const char_type* const str, const size_type str_len, const char_type ch) noexcept {
        return static_cast<size_type>(bs::strcount(str, str_len, ch));
    }
};

}
