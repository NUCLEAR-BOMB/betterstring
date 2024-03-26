#pragma once

#include <cstdint>
#include <type_traits>
#include <array>
#include <betterstring/type_traits.hpp>
#include <betterstring/char_traits.hpp>

namespace bs {

template<class Traits, std::uintmax_t Size>
class fixed_stringt {
    using traits_type = Traits;
    using value_type = typename traits_type::char_type;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = value_type*;
    using const_iterator = const value_type*;
    using size_type = typename traits_type::size_type;
    using difference_type = typename Traits::difference_type;

private:
    value_type arr_data[Size]{};
public:

    template<class It, std::enable_if_t<detail::is_random_access_iterator<It>, int> = 0>
    explicit constexpr fixed_stringt(It first, const size_type count) noexcept {
        BS_VERIFY(count == Size, "count must be the same as fixed string length");
        traits_type::copy(arr_data, detail::to_address(first), count);
    }
    template<class It, class End, std::enable_if_t<detail::is_random_access_iterator<It> && !std::is_convertible_v<End, size_type>, int> = 0>
    explicit constexpr fixed_stringt(It first, End last) {
        const size_type count = static_cast<size_type>(last - first);
        const const_pointer ptr = detail::to_address(first);
        BS_VERIFY(count == Size, "count must be the same as fixed string length");
        traits_type::copy(data(), ptr, count);
    }
    template<std::size_t N, std::enable_if_t<(N - 1) == Size, int> = 0>
    constexpr fixed_stringt(const value_type(&arr)[N]) noexcept {
        traits_type::copy(data(), arr, N - 1);
    }
    template<class U, std::size_t N, std::enable_if_t<
        detail::is_qualification_convertible<U, value_type> && (N == Size)
    , int> = 0>
    constexpr fixed_stringt(const std::array<U, N>& arr) noexcept {
        traits_type::copy(data(), arr.data(), N);
    }

    constexpr pointer data() noexcept { return arr_data; }
    constexpr const_pointer data() const noexcept { return arr_data; }

    constexpr size_type size() const noexcept { return Size; }
    constexpr size_type capacity() const noexcept { return Size; }
};

template<class Tr, std::size_t N1, std::size_t N2>
constexpr bool operator==(const fixed_stringt<Tr, N1>& left, const fixed_stringt<Tr, N2>& right) noexcept {
    if constexpr (N1 != N2) { return false; }
    else { return Tr::compare(left.data(), right.data(), N1) == 0; }
}
template<class Tr, std::size_t N1, std::size_t N2>
constexpr bool operator==(const fixed_stringt<Tr, N1>& left, const typename Tr::char_type(&right)[N2]) noexcept {
    static_assert(N2 != 0, "given non-null terminated array");
    if constexpr (N1 != (N2 - 1)) { return false; }
    else { return Tr::compare(left.data(), right, N1) == 0; }
}
template<class Tr, std::size_t N1, std::size_t N2>
constexpr bool operator==(const typename Tr::char_type(&left)[N1], const fixed_stringt<Tr, N2>& right) noexcept {
    static_assert(N1 != 0, "given non-null terminated array");
    if constexpr ((N1 - 1) != N2) { return false; }
    else { return Tr::compare(left, right.data(), N2) == 0; }
}

template<class Tr, std::size_t N1, std::size_t N2>
constexpr bool operator!=(const fixed_stringt<Tr, N1>& left, const fixed_stringt<Tr, N2>& right) noexcept {
    return !(left == right);
}
template<class Tr, std::size_t N1, std::size_t N2>
constexpr bool operator!=(const fixed_stringt<Tr, N1>& left, const typename Tr::char_type(&right)[N2]) noexcept {
    return !(left == right);
}
template<class Tr, std::size_t N1, std::size_t N2>
constexpr bool operator!=(const typename Tr::char_type(&left)[N1], const fixed_stringt<Tr, N2>& right) noexcept {
    return !(left == right);
}

template<std::size_t N>
using fixed_string = fixed_stringt<bs::char_traits<char>, N>;


}
