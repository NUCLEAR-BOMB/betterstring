
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <betterstring/functions.hpp>
#include <betterstring/detail/preprocessor.hpp>
#include <iterator>

namespace bs {

namespace detail {
    template<class T, class = void>
    inline constexpr bool has_size_method = false;
    template<class T>
    inline constexpr bool has_size_method<T, std::void_t<decltype(std::declval<const T&>().size())>> = true;

    template<class T>
    constexpr std::size_t size_or_1(const T& obj) noexcept {
        if constexpr (has_size_method<T>) {
            return obj.size();
        } else {
            return 1;
        }
    }
}

template<class String, class Separator>
struct splited_string {
public:
    using separator_type = Separator;
    using string_type = String;
    using size_type = typename string_type::size_type;

    constexpr splited_string(string_type str, separator_type sep) noexcept
        : string(str), separator(sep) {}

    class iterator {
    public:
        // iterator traits
        using difference_type = std::ptrdiff_t;
        using value_type = string_type;
        using size_type = typename string_type::size_type;
        using reference = void;
        using pointer = void;
        using iterator_category = std::input_iterator_tag;

        constexpr iterator(string_type str, separator_type sep) noexcept
            : string(str), separator(sep) {}

        constexpr string_type operator*() noexcept {
            current_end = string.find(separator, current_begin);
            return string(current_begin, current_end);
        }

        constexpr iterator& operator++() noexcept {
            current_begin = current_end + detail::size_or_1(separator);
            return *this;
        }

        constexpr bool operator!=(const iterator&) const noexcept {
            return current_end != string.size();
        }
        constexpr bool operator==(const iterator&) const noexcept {
            return current_end == string.size();
        }

    private:
        string_type string;
        separator_type separator;
        size_type current_begin = 0;
        size_type current_end = 0;
    };

    constexpr iterator begin() const noexcept { return iterator(string, separator); }
    constexpr iterator end() const noexcept { return begin(); }

    constexpr string_type nth(size_type index) const noexcept {
        size_type i = 0;
        for (; index > 0; --index) {
            i = string.find(separator, i) + detail::size_or_1(separator);
        }
        BS_VERIFY(i <= string.size(), "out of range");
        const size_type end = string.find(separator, i);
        return string(i, end);
    }
    constexpr size_type count() const noexcept {
        return string.count(separator) + 1;
    }

private:
    string_type string;
    separator_type separator;
};

}
