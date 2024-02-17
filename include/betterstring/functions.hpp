#pragma once

#include <cstring>
#include <cwchar>
#include <type_traits>
#include <algorithm>
#include <cstddef>

#include <betterstring/detail/preprocessor.hpp>
#include <betterstring/detail/cpu_features.hpp>
#include <betterstring/type_traits.hpp>

namespace bs {

namespace detail {
    template<class T, class = void>
    inline constexpr bool has_data_member = false;
    template<class T>
    inline constexpr bool has_data_member<T, std::void_t<decltype(std::declval<T&>().data())>>
        = bs::is_character<std::remove_cv_t<std::remove_pointer_t<decltype(std::declval<T&>().data())>>>;

    template<class T, class = void> struct has_c_str_member : std::false_type {};
    template<class T> struct has_c_str_member<T, std::void_t<decltype(std::declval<T&>().c_str())>> : std::true_type {};
}

template<class T>
constexpr auto* cstr(T& str) noexcept {
    if constexpr (is_character<std::remove_cv_t<std::remove_pointer_t<T>>>) {
        return str;
    } else if constexpr (detail::has_c_str_member<T>()) {
        return str.c_str();
    } else {
        static_assert(detail::always_false<T>, "cannot get underlying string data");
    }
}
template<class T, std::size_t N>
constexpr auto* cstr(T(&array)[N]) noexcept {
    return array;
}

template<class T>
constexpr auto* data(T& str) noexcept {
    if constexpr (is_character<std::remove_cv_t<std::remove_pointer_t<T>>>) {
        return str;
    } else if constexpr (detail::has_data_member<T>) {
        return str.data();
    } else if constexpr (detail::has_c_str_member<T>()) {
        return str.c_str();
    } else {
        static_assert(detail::always_false<T>, "cannot get underlying string data");
    }
}
template<class T, std::size_t N>
constexpr T* data(T(&array)[N]) noexcept {
    return array;
}

namespace detail {
    template<class T, class = void> struct has_size_method : std::false_type {};
    template<class T> struct has_size_method<T, std::void_t<decltype(std::declval<T>().size())>>
        : std::is_integral<decltype(std::declval<T>().size())> {};

}

template<class T>
constexpr auto array_size(const T& x) noexcept {
    if constexpr (is_character<T>) {
        return std::size_t(1);
    } else if constexpr (detail::has_size_method<T>()) {
        return x.size();
    } else {
        static_assert(detail::always_false<T>, "cannot get underlying container size");
    }
}

namespace detail {
    extern "C" std::size_t betterstring_strlen_avx2(const char*);
}

template<class T>
constexpr std::size_t strlen(const T* const str) noexcept {
    static_assert(is_character<T>);
    BS_VERIFY(str != nullptr, "str is null pointer");

    if (!detail::is_constant_evaluated()) {
        if constexpr (std::is_same_v<T, char>) {
            using detail::cpu_features;
            if (cpu_features.value & (cpu_features.AVX2 + cpu_features.BMI2)) {
                return detail::betterstring_strlen_avx2(str);
            }
            return std::strlen(str);
        } else if constexpr (std::is_same_v<T, wchar_t>) {
            return std::wcslen(str);
        }
    }
    std::size_t i = 0;
    while (str[i] != T()) ++i;
    return i;
}

namespace detail {
    template<class T> BS_FORCEINLINE
    constexpr std::size_t strlen_elision(const T* const str) noexcept {
        if (!detail::is_constant_evaluated()) {
            if constexpr (std::is_same_v<T, char>) {
                return std::strlen(str);
            } else if constexpr (std::is_same_v<T, wchar_t>) {
                return std::wcslen(str);
            }
        }
        std::size_t i = 0;
        while (str[i] != T()) ++i;
        return i;
    }
}

template<class T>
constexpr void strcopy(T* const dest, const T* const src, const std::size_t count) noexcept {
    static_assert(is_character<T>);
    BS_VERIFY(dest != nullptr, "dest or src is null pointer");
    BS_VERIFY(src != nullptr, "src is null pointer");
#if BS_HAS_BUILTIN(__builtin_wmemcpy)
    if constexpr (std::is_same_v<T, wchar_t>) {
        __builtin_wmemcpy(dest, src, count);
    } else
#endif
#if BS_HAS_BUILTIN(__builtin_memcpy)
    __builtin_memcpy(dest, src, count * sizeof(T));
#else
    if (detail::is_constant_evaluated()) {
        for (std::size_t i = 0; i < count; ++i) {
            dest[i] = src[i];
        }
    } else {
        if constexpr (std::is_same_v<T, wchar_t>) {
            std::wmemcpy(dest, src, count);
        } else {
            std::memcpy(dest, src, count * sizeof(T));
        }
    }
#endif
}

template<class T>
constexpr void strcopy(const T* const, const T* const, const std::size_t) noexcept = delete;

template<class T>
constexpr void strcopy(T* const dest, const std::size_t dest_size, const T* const src, const std::size_t count) noexcept {
    bs::strcopy(dest, src, std::min(dest_size, count));
}

template<class T, std::size_t N>
constexpr void strcopy(T* const dest, const std::size_t dest_size, const T(&src)[N]) noexcept {
    bs::strcopy(dest, src, std::min(dest_size, N - 1));
}

template<class T>
constexpr int strcomp(const T* const left, const T* const right, const std::size_t count) noexcept {
    static_assert(is_character<T>);
    BS_VERIFY(left != nullptr, "left is null pointer");
    BS_VERIFY(right != nullptr, "right is null pointer");
#if BS_HAS_BUILTIN(__builtin_wmemcmp) || defined(_MSC_VER)
    if constexpr (std::is_same_v<T, wchar_t>) {
        return __builtin_wmemcmp(left, right, count);
    } else
#endif
#if BS_HAS_BUILTIN(__builtin_memcmp) || defined(_MSC_VER)
    return __builtin_memcmp(left, right, count * sizeof(T));
#else
    {
    if (!detail::is_constant_evaluated()) {
        if constexpr (std::is_same_v<T, wchar_t>) {
            return std::wmemcmp(left, right, count);
        } else if constexpr (std::is_same_v<T, char>) {
            return std::memcmp(left, right, count);
        }
    }
    for (std::size_t i = 0; i < count; ++i) {
        if (left[i] < right[i]) return -1;
        if (left[i] > right[i]) return 1;
    }
    return 0;
    }
#endif
}

template<class T>
constexpr int strcomp(const T* const left, const std::size_t left_len, const T* const right, const std::size_t right_len) noexcept {
    BS_VERIFY(left != nullptr, "left is null pointer");
    BS_VERIFY(right != nullptr, "right is null pointer");
    if (left_len > right_len) return 1;
    if (left_len < right_len) return -1;
    return bs::strcomp(left, right, left_len);
}
template<class T, std::size_t N>
constexpr int strcomp(const T* const left, const std::size_t left_len, const T(&right)[N]) noexcept {
    return bs::strcomp(left, left_len, right, N - 1);
}

template<class T>
constexpr bool streq(const T* const left, const T* const right, const std::size_t count) noexcept {
    return bs::strcomp(left, right, count) == 0;
}
template<class T>
constexpr bool streq(const T* const left, const std::size_t left_len, const T* const right, const std::size_t right_len) noexcept {
    return bs::strcomp(left, left_len, right, right_len) == 0;
}
template<class T, std::size_t N>
constexpr bool streq(const T* const left, const std::size_t left_len, const T(&right)[N]) noexcept {
    return bs::strcomp(left, left_len, right, N - 1) == 0;
}

template<class T>
[[nodiscard]] constexpr T* strfind(
    T* const str, const std::size_t count, const detail::type_identity_t<T> ch
) noexcept {
    using pure_T = std::remove_cv_t<T>;
#if BS_HAS_BUILTIN(__builtin_char_memchr) || defined(_MSC_VER)
    if constexpr (std::is_same_v<pure_T, char>) {
        return const_cast<T*>(__builtin_char_memchr(str, static_cast<unsigned char>(ch), count));
    } else
#endif
#if BS_HAS_BUILTIN(__builtin_wmemchr) || defined(_MSC_VER)
    if constexpr (std::is_same_v<pure_T, wchar_t>) {
        return const_cast<T*>(__builtin_wmemchr(str, ch, count));
    } else
#endif
    if (detail::is_constant_evaluated()) {
        for (std::size_t i = 0; i < count; ++i) {
            if (str[i] == ch) return str + i;
        }
        return nullptr;
    } else {
        if constexpr (std::is_same_v<pure_T, char>) {
            return std::memchr(str, static_cast<unsigned char>(ch), count);
        } else if constexpr (std::is_same_v<pure_T, wchar_t>) {
            return std::wmemchr(str, ch, count);
        } else {
            T* const result = std::find(str, str + count, ch);
            return result == str + count ? nullptr : result;
        }
    }
}

template<class T, std::size_t N>
constexpr T* strfind(T (&str)[N], const detail::type_identity_t<T> ch) noexcept {
    return bs::strfind(str, N - 1, ch);
}

template<class T>
constexpr T* strfind(T* const haystack, const std::size_t count, const detail::type_identity_t<T>* const needle, const std::size_t needle_len) noexcept {
    if (needle_len > count) return nullptr;
    if (needle_len == 0) return haystack;

    const auto match_end = haystack + (count - needle_len) + 1;
    for (T* match_try = haystack;; ++match_try) {
        match_try = bs::strfind(match_try, static_cast<std::size_t>(match_end - match_try), needle[0]);
        if (match_try == nullptr) return nullptr;

        // the needle may be aligned for std::memcmp,
        // so we compare the entire string even though
        // we know that the first character fits
        if (bs::strcomp(match_try, needle, needle_len) == 0) {
            return match_try;
        }
    }
    BS_UNREACHABLE();
}

template<class T, std::size_t N>
constexpr T* strfind(T* const haystack, const std::size_t count, const detail::type_identity_t<T>(&needle)[N]) noexcept {
    return strfind(haystack, count, needle, N - 1);
}

template<class Haystack, class Needle>
constexpr auto strfind(Haystack& haystack, const Needle& needle) noexcept
    -> decltype(bs::strfind(haystack.data(), haystack.size(), needle.data(), needle.size())) {
    return bs::strfind(haystack.data(), haystack.size(), needle.data(), needle.size());
}
template<class Haystack, class Needle>
constexpr auto strfind(Haystack& haystack, const Needle needle) noexcept
    -> decltype(bs::strfind(haystack.data(), haystack.size(), needle)) {
    return bs::strfind(haystack.data(), haystack.size(), needle);
}

namespace detail {
    const char* strrfind_string_impl(const char*, std::size_t, const char*, std::size_t);
}

template<class T>
constexpr T* strrfind(T* const haystack, const std::size_t count, const detail::type_identity_t<T>* const needle, const std::size_t needle_len) noexcept {
#if 0
    if constexpr (std::is_same_v<std::remove_const_t<T>, char>) {
        if (!detail::is_constant_evaluated()) {
            return const_cast<char*>(detail::strrfind_string_impl(haystack, count, needle, needle_len));
        }
    }
#endif
    if (needle_len > count) return nullptr;
    if (needle_len == 0) return haystack + count;

    for (auto match_try = haystack + (count - needle_len);; --match_try) {
        if (bs::strcomp(match_try, needle, needle_len) == 0) {
            return match_try;
        }
        if (match_try == haystack) return nullptr;
    }
    BS_UNREACHABLE();
}

template<class T, std::size_t N>
constexpr T* strrfind(T* const haystack, const std::size_t count, const detail::type_identity_t<T>(&needle)[N]) noexcept {
    return bs::strrfind(haystack, count, needle, N - 1);
}

namespace detail {
    extern "C" const char* betterstring_strrfind_char_avx2(const char*, std::size_t, char);
}

template<class T>
constexpr T* strrfind(T* const str, const std::size_t count, const detail::type_identity_t<T> ch) noexcept {
    static_assert(is_character<std::remove_const_t<T>>);
    using type = std::remove_const_t<T>;
    if (!detail::is_constant_evaluated()) {
        if constexpr (std::is_same_v<type, char>) {
            using detail::cpu_features;
            if (cpu_features.value & (cpu_features.AVX2 + cpu_features.BMI2)) {
                return const_cast<char*>(detail::betterstring_strrfind_char_avx2(str, count, ch));
            } else {
                for (std::size_t i = count; i > 0; --i) {
                    if (str[i - 1] == ch) return str + i - 1;
                }
                return nullptr;
            }
        }
    }

    for (std::size_t i = count; i > 0; --i) {
        if (str[i - 1] == ch) return str + i - 1;
    }
    return nullptr;
}

template<class T, std::size_t N>
constexpr T* strrfind(T (&str)[N], const detail::type_identity_t<T> ch) noexcept {
    return bs::strrfind(str, N - 1, ch);
}

template<class T>
constexpr void strfill(T* const dest, const std::size_t count, const detail::type_identity_t<T> ch) noexcept {
    BS_VERIFY(dest != nullptr, "dest is null pointer");
#if BS_HAS_BUILTIN(__builtin_wmemset) && (defined(__GNUC__) || defined(__GNUG__)) && !defined(__clang__)
    if constexpr (std::is_same_v<T, wchar_t>) {
        __builtin_wmemset(dest, ch, count);
    } else
#endif
#if BS_HAS_BUILTIN(__builtin_memset) && (defined(__GNUC__) || defined(__GNUG__)) && !defined(__clang__)
    if constexpr (std::is_same_v<T, char>) {
        __builtin_memset(dest, static_cast<unsigned char>(ch), count);
    } else
#endif
    if (detail::is_constant_evaluated()) {
        for (std::size_t i = 0; i < count; ++i) {
            dest[i] = ch;
        }
    } else {
        if constexpr (std::is_same_v<T, char>) {
            std::memset(dest, static_cast<unsigned char>(ch), count);
        } else if constexpr (std::is_same_v<T, wchar_t>) {
            std::wmemset(dest, ch, count);
        } else {
            std::fill_n(dest, count, ch);
        }
    }
}

template<class T>
constexpr void strmove(T* const dest, const T* const src, const std::size_t count) noexcept {
    BS_VERIFY(dest != nullptr, "dest is null pointer");
    BS_VERIFY(src != nullptr, "src is null pointer");
#if BS_HAS_BUILTIN(__builtin_wmemmove) || defined(_MSC_VER)
    if constexpr (std::is_same_v<T, wchar_t>) {
        __builtin_wmemmove(dest, src, count);
    } else
#endif
#if BS_HAS_BUILTIN(__builtin_memmove) && defined(_MSC_VER)
    __builtin_memmove(dest, src, count * sizeof(T));
#else
    if (detail::is_constant_evaluated()) {
        bool loop_forward = true;
        for (const T* it = src; it != src + count; ++it) {
            if (it == dest) {
                loop_forward = false;
                break;
            }
        }

        if (loop_forward) {
            bs::strcopy(dest, src, count);
        } else {
            for (std::size_t i = count; i > 0; --i) {
                dest[i - 1] = src[i - 1];
            }
        }
    } else {
        if constexpr (std::is_same_v<T, wchar_t>) {
            std::wmemmove(dest, src, count);
        } else {
            std::memmove(dest, src, count * sizeof(T));
        }
    }
#endif
}
template<class T>
constexpr void strmove(const T* const, const T* const, const std::size_t) noexcept = delete;

template<class T>
constexpr void strmove(T* const dest, const std::size_t dest_size, const T* const src, const std::size_t count) noexcept {
    return bs::strmove(dest, src, std::min(dest_size, count));
}

namespace detail {
    extern "C" std::size_t betterstring_strcount_char_avx2(const char*, std::size_t, char);
}

template<class T>
constexpr std::size_t strcount(const T* str, const std::size_t count, const detail::type_identity_t<T> ch) noexcept {
    static_assert(is_character<T>);
    if (!detail::is_constant_evaluated()) {
        if constexpr (std::is_same_v<T, char>) {
            using detail::cpu_features;
            if (cpu_features.value & (cpu_features.AVX2 + cpu_features.BMI2 + cpu_features.POPCNT)) {
                return detail::betterstring_strcount_char_avx2(str, count, ch);
            }
        }
    }
    std::size_t result = 0;
    const auto match_end = str + count;
    for (;; ++str) {
        str = bs::strfind(str, static_cast<std::size_t>(match_end - str), ch);
        if (str == nullptr) { break; }
        ++result;
    }
    return result;
}

}
