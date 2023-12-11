#pragma once

#include <cstring>
#include <cwchar>
#include <type_traits>

#include <betterstring/detail/preprocessor.hpp>

namespace bs {

namespace detail {
    constexpr bool is_constant_evaluated() noexcept {
#ifdef __cpp_lib_is_constant_evaluated
        return std::is_constant_evaluated();
#elif defined(__clang__) || defined(__GNUC__) || defined(__GNUG__) || defined(_MSC_VER)
        return __builtin_is_constant_evaluated();
#else
        return false;
#endif
    }

    template<class>
    inline constexpr bool always_false = false;

    template<class T>
    struct type_identity { using type = T; };

    template<class T>
    using type_identity_t = typename type_identity<T>::type;

    template<class T>
    struct is_character_impl : std::false_type {};
    template<> struct is_character_impl<char> : std::true_type {};
    template<> struct is_character_impl<wchar_t> : std::true_type {};
    template<> struct is_character_impl<char16_t> : std::true_type {};
    template<> struct is_character_impl<char32_t> : std::true_type {};
#if __cplusplus >= 202002L
    template<> struct is_character_impl<char8_t> : std::true_type {};
#endif
}

template<class T>
inline constexpr bool is_character = detail::is_character_impl<T>();

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

template<class T>
constexpr std::size_t strlen(const T* const str) noexcept {
    static_assert(is_character<T>);
    BS_VERIFY(str != nullptr, "str is null pointer");
    if constexpr (std::is_same_v<T, char>) {
#if BS_HAS_BUILTIN(__builtin_strlen) || defined(_MSC_VER)
        return __builtin_strlen(str);
#endif
    } else
#if BS_HAS_BUILTIN(__builtin_wcslen) || defined(_MSC_VER)
    if constexpr (std::is_same_v<T, wchar_t>) {
        return __builtin_wcslen(str);
    } else
#endif
    {
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
    BS_VERIFY(left != nullptr && right != nullptr, "left or right is null pointer");
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
    BS_VERIFY(left != nullptr && right != nullptr, "left or right is null pointer");
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
[[nodiscard]] constexpr bool empty(const T* const str) noexcept {
    return str[0] == '\0';
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
    extern const char* strrfind_impl(const char* const haystack, const std::size_t count, const char* const needle, const std::size_t needle_len) noexcept;
}

template<class T>
constexpr T* strrfind(T* const haystack, const std::size_t count, const detail::type_identity_t<T>* const needle, const std::size_t needle_len) noexcept {
    if constexpr (std::is_same_v<std::remove_const_t<T>, char>) {
        if (!detail::is_constant_evaluated()) {
            return const_cast<T*>(bs::detail::strrfind_impl(haystack, count, needle, needle_len));
        }
    }
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

#if 0
namespace detail {
    template<class T>
    const T* avx2_strrfind(const T* const string, std::size_t count, const T search_character) {
        const T* char_ptr = string + count - 1;
        // align
        for (; count > 0; --count, --char_ptr) {
            if (*char_ptr == search_character) return char_ptr;
            if ((reinterpret_cast<std::uintptr_t>(char_ptr) & (sizeof(__m256) - 1)) == 0) {
                --count;
                break;
            }
        }
        const __m256i search_char32 = _mm256_set1_epi8(search_character);
        for (; count >= sizeof(__m256); count -= sizeof(__m256)) {
            char_ptr -= sizeof(__m256);
            const __m256i char32 = _mm256_load_si256(reinterpret_cast<const __m256i*>(char_ptr));
            const __m256i cmpeq_result = _mm256_cmpeq_epi8(char32, search_char32);
            const std::uint32_t cmp_mask = static_cast<std::uint32_t>(_mm256_movemask_epi8(cmpeq_result));
            if (cmp_mask != 0) {
                return &char_ptr[sizeof(__m256) - detail::lzcnt(cmp_mask) - 1];
            }
        }
        for (; count != 0; --count) {
            if (*--char_ptr == search_character) return char_ptr;
        }
        return nullptr;
    }
}
#endif

template<class T>
constexpr T* strrfind(T* const str, const std::size_t count, const detail::type_identity_t<T> ch) noexcept {
    static_assert(is_character<std::remove_const_t<T>>);
#if 0
    if (!detail::is_constant_evaluated()) {
        return const_cast<T*>(detail::avx2_strrfind(str, count, ch));
    }
#endif
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
    static constexpr const char_type* findstr(const char_type* const str, const std::size_t count, const char_type* const substr, const std::size_t substr_len) noexcept {
        return bs::strfind(str, count, substr, substr_len);
    }
    static constexpr const char_type* rfind(const char_type* const str, const std::size_t count, const char_type ch) noexcept {
        return bs::strrfind(str, count, ch);
    }
    static constexpr const char_type* rfindstr(const char_type* const str, const std::size_t count, const char_type* const substr, const std::size_t substr_len) noexcept {
        return bs::strrfind(str, count, substr, substr_len);
    }
};

}
