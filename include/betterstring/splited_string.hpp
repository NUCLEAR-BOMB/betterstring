#pragma once

#include <betterstring/functions.hpp>
#include <iterator>

namespace bs {

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

        constexpr iterator& operator=(const iterator&) noexcept = default;

        constexpr string_type operator*() noexcept {
            current_end = string.find(separator);
            return string(0, current_end);
        }

        constexpr iterator& operator++() noexcept {
            const auto remove_num = string.size() != current_end ? bs::array_size(separator) : 0;
            string.remove_prefix(current_end + remove_num);
            return *this;
        }
        constexpr bool operator!=(const iterator&) const noexcept {
            return !string.empty();
        }
        constexpr bool operator==(const iterator& x) const noexcept { return !(*this == x); }

    private:
        string_type string;
        separator_type separator;
        size_type current_end = 0;
    };

    constexpr iterator begin() const noexcept { return iterator(string, separator); }
    constexpr iterator end() const noexcept { return begin(); }

    constexpr string_type operator[](size_type index) const noexcept {
        size_type i = 0;
        for (; index > 0; --index) {
            i = string.find(separator, i) + bs::array_size(separator);
        }
        const auto end = string.find(separator, i);
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
