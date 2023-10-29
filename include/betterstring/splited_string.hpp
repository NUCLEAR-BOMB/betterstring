#pragma once

#include <betterstring/functions.hpp>

namespace bs {

template<class Separator, class String>
struct splited_string {
public:
    using separator_type = Separator;
    using string_type = String;
    using size_type = typename String::size_type;

    constexpr splited_string(string_type str, separator_type sep) noexcept
        : string(str), separator(sep) {}

    struct iterator_end {};

    class iterator {
    public:
        constexpr iterator(const string_type str, const separator_type sep) noexcept
            : string(str), separator(sep), current_end(0) {}

        constexpr string_type operator*() noexcept {
            current_end = string.find(separator);
            return string(0, current_end);
        }
        constexpr iterator& operator++() noexcept {
            const auto remove_num = string.size() != current_end ? separator.size() : 0;
            string.remove_prefix(current_end + remove_num);
            return *this;
        }
        constexpr bool operator!=(const iterator_end) const noexcept {
            return !string.empty();
        }

    private:
        string_type string;
        separator_type separator;
        size_type current_end;
    };

    constexpr iterator begin() const noexcept { return iterator(string, separator); }
    constexpr iterator_end end() const noexcept { return iterator_end(); }

    constexpr string_type operator[](size_type index) const noexcept {
        size_type i = 0;
        for (; index > 0; --index) {
            i = string.find(separator, i) + separator.size();
        }
        const auto end = string.find(separator, i);
        return string(i, end);
    }

private:
    string_type string;
    separator_type separator;
};

}
