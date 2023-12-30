#pragma once

#include <type_traits>

namespace bs::detail {

template<class T1, class T2>
constexpr bool cmp_equal(const T1 a, const T2 b) noexcept {
    if constexpr (std::is_signed_v<T1> == std::is_signed_v<T2>) {
        return a == b;
    } else if constexpr (std::is_signed_v<T1>) {
        return a >= 0 && std::make_unsigned_t<T1>(a) == b;
    } else {
        return b >= 0 && std::make_unsigned_t<T2>(b) == a;
    }
}
template<class T1, class T2>
constexpr bool cmp_not_equal(const T1 a, const T2 b) noexcept {
    return !cmp_equal(a, b);
}
template<class T1, class T2>
constexpr bool cmp_less(const T1 a, const T2 b) noexcept {
    if constexpr (std::is_signed_v<T1> == std::is_signed_v<T2>) {
        return a < b;
    } else if constexpr (std::is_signed_v<T1>) {
        return a < 0 || std::make_unsigned_t<T1>(a) < b;
    } else {
        return b >= 0 && a < std::make_unsigned_t<T2>(b);
    }
}
template<class T1, class T2>
constexpr bool cmp_greater(const T1 a, const T2 b) noexcept {
    return cmp_less(b, a);
}
template<class T1, class T2>
constexpr bool cmp_less_equal(const T1 a, const T2 b) noexcept {
    return !cmp_less(b, a);
}
template<class T1, class T2>
constexpr bool cmp_greater_equal(const T1 a, const T2 b) noexcept {
    return !cmp_less(a, b);
}

}
