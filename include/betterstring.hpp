#pragma once

#include <string>
#include <iterator>
#include <type_traits>
#include <memory>

namespace bs
{

namespace detail {
    template<class T, class = void>
    inline constexpr bool has_pointer_traits_to_address = false;
    template<class T>
    inline constexpr bool has_pointer_traits_to_address<T, std::void_t<
        decltype(std::pointer_traits<T>::to_address(std::declval<T&>()))
    >> = true;

    template<class T>
    constexpr T* to_address(T* ptr) noexcept {
        static_assert(!std::is_function_v<T>);
        return ptr;
    }
    template<class T>
    constexpr auto to_address(const T& fancy_ptr) noexcept {
        if constexpr (has_pointer_traits_to_address<T>) {
            return std::pointer_traits<T>::to_address(fancy_ptr);
        } else {
            return to_address(fancy_ptr.operator->());
        }
    }
}

template<class T>
struct char_traits : std::char_traits<T> {
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
};

template<class Traits = char_traits<char>>
class string_view {
public:
    using traits_type            = Traits;
    using value_type             = typename Traits::char_type;
    using pointer                = value_type*;
    using const_pointer          = const value_type*;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using const_iterator         = const value_type*;
    using iterator               = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator       = const_reverse_iterator;
    using size_type              = typename Traits::size_type;
    using difference_type        = typename Traits::difference_type;

    constexpr string_view() noexcept : string_data(nullptr), string_size(0) {}

    constexpr string_view(const string_view&) noexcept = default;

    constexpr string_view(const const_pointer str, const size_type count)
        : string_data(str), string_size(count) {}

    constexpr string_view(const const_pointer ntstr) noexcept
        : string_data(ntstr), string_size(traits_type::length(ntstr)) {}

    template<class Begin, class End = Begin, std::enable_if_t<
        !std::is_convertible_v<End, size_type>
    , int> = 0>
    constexpr string_view(Begin first, End last) noexcept(noexcept(last - first))
        : string_data(detail::to_address(first))
        , string_size(static_cast<size_type>(last - first)) {}

    constexpr string_view(std::nullptr_t) = delete;

    constexpr string_view& operator=(const string_view&) noexcept = default;

    constexpr const_iterator begin() const noexcept { return data(); }
    constexpr const_iterator end() const noexcept { return data() + size(); }
    constexpr const_iterator cbegin() const noexcept { return begin(); }
    constexpr const_iterator cend() const noexcept { return end(); }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return std::reverse_iterator(begin());
    }
    constexpr const_reverse_iterator rend() const noexcept {
        return std::reverse_iterator(end());
    }
    constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    constexpr const_reverse_iterator crend() const noexcept { return rend(); }

    constexpr const_reference operator[](const size_type index) const noexcept {
        return data()[index];
    }

    constexpr const_reference front() const noexcept { return data()[0]; }
    constexpr const_reference back() const noexcept { return data()[size() - 1]; }

    constexpr const_pointer data() const noexcept { return string_data; }

    constexpr size_type size() const noexcept { return string_size; }
    constexpr size_type length() const noexcept { return size(); }

    constexpr size_type max_size() const noexcept { return static_cast<size_type>(-1); }

    [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; }

    constexpr void remove_prefix(const size_type count) noexcept {
        string_data += count;
        string_size -= count;
    }
    constexpr void remove_suffix(const size_type count) noexcept {
        string_size -= count;
    }

    constexpr string_view operator()(const size_type start, const size_type finish) const noexcept {
        return string_view(data() + start, finish - start);
    }

    constexpr string_view substr(const size_type position) const noexcept {
        return (*this)(position, size());
    }
    constexpr string_view substr(const size_type position, const size_type count) const noexcept {
        return (*this)(position, position + count);
    }

    constexpr bool starts_with(const string_view str) const noexcept {
        if (size() < str.size()) return false;
        return traits_type::compare(data(), str.data(), str.size()) == 0;
    }
    constexpr bool starts_with(const value_type ch) const noexcept {
        return !empty() && traits_type::eq(front(), ch);
    }

    constexpr bool ends_with(const string_view str) const noexcept {
        if (size() < str.size()) return false;
        return traits_type::compare(data() + (size() - str.size()), str.data(), str.size()) == 0;
    }
    constexpr bool ends_with(const value_type ch) const noexcept {
        return !empty() && traits_type::eq(back(), ch);
    }

private:
    static constexpr int trait_cmp(const string_view l, const string_view r) noexcept {
        if (l.size() > r.size()) return 1;
        if (l.size() < r.size()) return -1;
        return Traits::compare(l.data(), r.data(), l.size());
    }
public:

    friend constexpr bool operator==(const string_view l, const string_view r) noexcept {
        return trait_cmp(l, r) == 0; }
    friend constexpr bool operator!=(const string_view l, const string_view r) noexcept {
        return trait_cmp(l, r) != 0; }
    friend constexpr bool operator>(const string_view l, const string_view r) noexcept {
        return trait_cmp(l, r) > 0; }
    friend constexpr bool operator>=(const string_view l, const string_view r) noexcept {
        return trait_cmp(l, r) >= 0; }
    friend constexpr bool operator<(const string_view l, const string_view r) noexcept {
        return trait_cmp(l, r) < 0; }
    friend constexpr bool operator<=(const string_view l, const string_view r) noexcept {
        return trait_cmp(l, r) <= 0; }

private:
    const_pointer string_data;
    size_type string_size;
};

}
