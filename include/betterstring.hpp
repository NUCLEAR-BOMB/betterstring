#pragma once

#include <iterator>
#include <type_traits>
#include <memory>
#include <cstring>
#include <algorithm>
#include <cwchar>

#ifndef BS_DONT_INCLUDE_STRING
#include <string>
#endif

#ifdef __has_builtin
    #define BS_HAS_BUILTIN(x) __has_builtin(x)
#else
    #define BS_HAS_BUILTIN(x) 0
#endif

#if BS_HAS_BUILTIN(__builtin_debugtrap)
    #define BS_DEBUG_BREAK __builtin_debugtrap()
#elif defined(__GNUC__) || defined(__GNUG__)
    #define BS_DEBUG_BREAK __builtin_trap()
#elif defined(_MSC_VER)
    #define BS_DEBUG_BREAK __debugbreak()
#else
    #include <csignal>
    #if defined(SIGTRAP)
        #define BS_DEBUG_BREAK std::raise(SIGTRAP)
    #else
        #define BS_DEBUG_BREAK std::raise(SIGABRT)
    #endif
#endif

#define BS_ABORT(expression, message) \
    ((void)std::fprintf(stderr, "%s:%d: assertion '%s' failed: %s\n", __FILE__, __LINE__, #expression, message), (void)BS_DEBUG_BREAK)

#if BS_HAS_BUILTIN(__builtin_assume)
    #define BS_ASSUME(expression) __builtin_assume(expression)
#elif defined(__GNUC__) || defined(__GNUG__)
    #define BS_ASSUME(expression) \
        ((expression) ? (void)0 : (void)__builtin_unreachable())
#elif defined(_MSC_VER)
    #define BS_ASSUME(expression) __assume(expression)
#else
    #define BS_ASSUME(expression) ((void)0)
#endif

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #define BS_CONST_FN [[gnu::const]]
#elif defined(_MSC_VER)
    #define BS_CONST_FN __declspec(noalias)
#else
    #define BS_CONST_FN
#endif

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #define BS_UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
    #define BS_UNREACHABLE() __assume(false)
#else
    #define BS_UNREACHABLE()
#endif

#ifndef NDEBUG
    #if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
        #define BS_VERIFY(expression, message) \
            (__builtin_expect((expression), 1) ? (void)0 : BS_ABORT(expression, message))
    #else
        #define BS_VERIFY(expression, message) \
            ((expression) ? (void)0 : BS_ABORT(expression, message))
    #endif
#else
    #define BS_VERIFY(expression, message) BS_ASSUME(expression)
#endif

namespace bs
{

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

    template<class T>
    struct type_identity { using type = T; };

    template<class T>
    using type_identity_t = typename type_identity<T>::type;

    template<class T>
    struct is_character_impl : std::false_type {};
    template<> struct is_character_impl<char> : std::true_type {};
    template<> struct is_character_impl<const char> : std::true_type {};
    template<> struct is_character_impl<wchar_t> : std::true_type {};
    template<> struct is_character_impl<const wchar_t> : std::true_type {};
    template<> struct is_character_impl<char16_t> : std::true_type {};
    template<> struct is_character_impl<const char16_t> : std::true_type {};
    template<> struct is_character_impl<char32_t> : std::true_type {};
    template<> struct is_character_impl<const char32_t> : std::true_type {};
#if __cplusplus >= 202002L
    template<> struct is_character_impl<char8_t> : std::true_type {};
    template<> struct is_character_impl<const char8_t> : std::true_type {};
#endif
}

template<class T>
inline constexpr bool is_character = detail::is_character_impl<T>();

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

template<class T>
constexpr T* strrfind(T* const haystack, const std::size_t count, const detail::type_identity_t<T>* const needle, const std::size_t needle_len) noexcept {
    if (needle_len > count) return nullptr;
    if (needle_len == 0) return haystack;

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

template<class T>
constexpr T* strrfind(T* const str, const std::size_t count, const detail::type_identity_t<T> ch) noexcept {
    static_assert(is_character<T>);
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
constexpr T* cstr(T* const str) noexcept {
    static_assert(is_character<T>);
    return str;
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

    template<class T>
    class char_bitmap {
    public:
        constexpr bool mark(const T* first, const T* const last) noexcept {
            BS_VERIFY(first != nullptr && last != nullptr, "first or last is null pointer");
            for (; first != last; ++first) {
                const auto ch = static_cast<unsigned char>(*first);
                if constexpr (sizeof(T) != 1) if (ch >= 256) return false;
                matches[ch] = true;
            }
            return true;
        }
        constexpr bool match(const T ch) const noexcept {
            return matches[static_cast<unsigned char>(ch)];
        }

    private:
        bool matches[256] = {};
    };
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

template<class CharT>
class find_result {
public:
    using size_type = std::size_t;

    constexpr find_result(const CharT* string_data, const size_type string_size, const CharT* find_ptr) noexcept
        : string_data(string_data), string_size(string_size), find_ptr(find_ptr) {}

    constexpr size_type index() const noexcept {
        BS_VERIFY(find_ptr != nullptr, "index() is called when the result is not found");
        return static_cast<size_type>(find_ptr - string_data);
    }
    constexpr size_type index_or_end() const noexcept {
        if (find_ptr == nullptr) return string_size;
        return static_cast<size_type>(find_ptr - string_data);
    }
    constexpr size_type index_or_npos() const noexcept {
        if (find_ptr == nullptr) return static_cast<size_type>(-1);
        return static_cast<size_type>(find_ptr - string_data);
    }
    constexpr operator size_type() const noexcept {
        return index_or_end();
    }

    constexpr const CharT* ptr() const noexcept {
        BS_VERIFY(find_ptr != nullptr, "ptr() is called when the result is not found");
        return find_ptr;
    }
    constexpr const CharT* ptr_or_end() const noexcept {
        if (find_ptr == nullptr) return string_data + string_size;
        return find_ptr;
    }
    constexpr const CharT* ptr_or_null() const noexcept {
        return find_ptr;
    }

    constexpr bool found() const noexcept {
        return find_ptr != nullptr;
    }

private:
    const CharT* string_data;
    size_type string_size;
    const CharT* find_ptr;
};

template<class>
struct splited_string;

struct slice {
    using index_type = std::ptrdiff_t;

    constexpr slice(const index_type stop) noexcept
        : start(0), stop(stop) {}
    constexpr slice(const index_type start, const index_type stop) noexcept
        : start(start), stop(stop) {}

    index_type start;
    index_type stop;
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
        : string_data(str), string_size(count) {
        BS_VERIFY(count == 0 || str != nullptr, "null pointer with non-zero size");
    }

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

private:
    template<class T>
    constexpr size_type to_index(const T integer) const noexcept {
        return static_cast<size_type>(integer < T(0) ? integer + static_cast<T>(size()) : integer);
    }
public:

    template<class T, std::enable_if_t<std::is_unsigned_v<T>, int> = 0>
    constexpr const_reference operator[](const T index) const noexcept {
        BS_VERIFY(index < size(), "out of range");
        return data()[index];
    }
    template<class T, std::enable_if_t<std::is_signed_v<T>, int> = 0>
    constexpr const_reference operator[](const T index) const noexcept {
        const T ssize = static_cast<T>(size());
        BS_VERIFY(index < ssize && index >= -ssize, "out of range");
        return data()[to_index(index)];
    }

    constexpr const_reference front() const noexcept {
        BS_VERIFY(!empty(), "cannot access the first element from an empty string");
        return data()[0];
    }
    constexpr const_reference back() const noexcept {
        BS_VERIFY(!empty(), "cannot access the last element from an empty string");
        return data()[size() - 1];
    }

    BS_CONST_FN constexpr const_pointer data() const noexcept { return string_data; }

    BS_CONST_FN constexpr size_type size() const noexcept { return string_size; }
    BS_CONST_FN constexpr size_type length() const noexcept { return size(); }

    BS_CONST_FN constexpr size_type max_size() const noexcept { return static_cast<size_type>(-1); }

    [[nodiscard]] BS_CONST_FN constexpr bool empty() const noexcept { return size() == 0; }

    constexpr void remove_prefix(const size_type count) noexcept {
        BS_VERIFY(count <= size(), "the number of characters to be removed from the start of the string exceeds the length of the string");
        string_data += count;
        string_size -= count;
    }
    constexpr void remove_suffix(const size_type count) noexcept {
        BS_VERIFY(count <= size(), "the number of characters to be removed from the end of the string exceeds the length of the string");
        string_size -= count;
    }

private:
    struct slice_end_t {
        constexpr slice_end_t() noexcept = default;
    };

    template<class T1, class T>
    constexpr T1 unwrap_slice_finish(const T finish) const noexcept {
        if constexpr (std::is_same_v<T, slice_end_t>) { return static_cast<T1>(size()); }
        else { return static_cast<T1>(finish); }
    }
public:

    template<class T1, class T2 = slice_end_t, std::enable_if_t<std::is_unsigned_v<T1>, int> = 0>
    constexpr string_view operator()(const T1 start, const T2 raw_finish) const noexcept {
        const auto finish = unwrap_slice_finish<T1>(raw_finish);
        BS_VERIFY(start < size() && finish <= size(), "slice out of range");
        return string_view(data() + start, finish - start);
    }
    template<class T1, class T2 = slice_end_t, std::enable_if_t<std::is_signed_v<T1>, int> = 0>
    constexpr string_view operator()(const T1 start, const T2 raw_finish) const noexcept {
        const auto finish = unwrap_slice_finish<T1>(raw_finish);
        const T1 ssize = static_cast<T1>(size());
        BS_VERIFY(start < ssize && start >= -ssize && finish <= ssize && finish > -ssize,
            "slice out of range");
        return string_view(data() + to_index(start), to_index(finish) - to_index(start));
    }

    constexpr string_view operator[](const slice sl) const noexcept {
        const auto ssize = static_cast<typename slice::index_type>(size());
        BS_VERIFY(sl.start < ssize && sl.start >= -ssize && sl.stop <= ssize && sl.stop > -ssize,
            "slice out of range");
        return string_view(data() + to_index(sl.start), to_index(sl.stop) - to_index(sl.start));
    }

    constexpr string_view substr(const size_type position) const noexcept {
        BS_VERIFY(position <= size(), "the start position of the substring exceeds the length of the string");
        return (*this)(position, size());
    }
    constexpr string_view substr(const size_type position, const size_type count) const noexcept {
        BS_VERIFY(position <= size(), "the start position of the substring exceeds the length of the string");
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
    using this_find_result = bs::find_result<value_type>;
    constexpr this_find_result make_find_r(const value_type* const ptr, const size_type end) const noexcept {
        return this_find_result(data(), end, ptr);
    }
    constexpr this_find_result make_find_r(const value_type* const ptr) const noexcept {
        return this_find_result(data(), size(), ptr);
    }
    constexpr this_find_result make_find_r(const size_type index) const noexcept {
        return this_find_result(data(), size(), data() + index);
    }
public:

    constexpr this_find_result find(const string_view str, const size_type start = 0) const noexcept {
        return this->find(str, start, size());
    }
    constexpr this_find_result find(const string_view str, const size_type start, const size_type end) const noexcept {
        BS_VERIFY(end <= size(), "end is out of range");
        return make_find_r(traits_type::findstr(data() + start, end - start, str.data(), str.size()), end);
    }

    constexpr this_find_result find(const value_type ch, const size_type start = 0) const noexcept {
        return this->find(ch, start, size());
    }
    constexpr this_find_result find(const value_type ch, const size_type start, const size_type end) const noexcept {
        BS_VERIFY(end <= size(), "end is out of range");
        if (start >= end) return make_find_r(nullptr, end);
        return make_find_r(traits_type::find(data() + start, end - start, ch), end);
    }

    constexpr this_find_result rfind(const value_type ch) const noexcept {
        return this->rfind(ch, size(), 0);
    }
    constexpr this_find_result rfind(const value_type ch, const size_type start, const size_type end = 0) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        if (start <= end) return make_find_r(nullptr, end);
        return make_find_r(traits_type::rfind(data() + end, start - end, ch), end);
    }

    constexpr this_find_result rfind(const string_view str) const noexcept {
        return this->rfind(str, size(), 0);
    }
    constexpr this_find_result rfind(const string_view str, const size_type start, const size_type end = 0) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        return make_find_r(traits_type::rfindstr(data() + end, start - end, str.data(), str.size()), end);
    }

    constexpr this_find_result find_first_of(const value_type ch) const noexcept {
        return this->find(ch);
    }
    constexpr this_find_result find_first_of(const string_view str) const noexcept {
        detail::char_bitmap<value_type> bitmap;
        if (!bitmap.mark(str.data(), str.data() + str.size())) {
            const auto match_end = data() + size();
            for (auto match_try = data(); match_try < match_end; ++match_try) {
                if (traits_type::find(str.data(), str.size(), *match_try)) {
                    return make_find_r(match_try);
                }
            }
            return make_find_r(nullptr);
        }
        for (size_type i = 0; i < size(); ++i) {
            if (bitmap.match(data()[i])) return make_find_r(i);
        }
        return make_find_r(nullptr);
    }

    constexpr bool contains(const value_type ch) const noexcept {
        return traits_type::find(data(), size(), ch) != nullptr;
    }
    constexpr bool contains(const string_view str) const noexcept {
        return traits_type::findstr(data(), size(), str.data(), str.size()) != nullptr;
    }

    constexpr splited_string<traits_type> split(const string_view separator) const noexcept {
        return splited_string<traits_type>(*this, separator);
    }

    constexpr size_type count(const value_type ch) const noexcept {
        size_type result = 0;
        const auto match_end = data() + size();
        for (auto match_try = data();; ++match_try) {
            match_try = traits_type::find(match_try, static_cast<size_type>(match_end - match_try), ch);
            if (match_try == nullptr) return result;
            ++result;
        }
        BS_UNREACHABLE();
    }
    constexpr size_type count(const string_view str) const noexcept {
        if (str.empty()) return this->size() + 1;

        size_type result = 0;
        const auto match_end = data() + size();
        for (auto match_try = data();; ++match_try) {
            match_try = traits_type::findstr(match_try, static_cast<size_type>(match_end - match_try), str.data(), str.size());
            if (match_try == nullptr) return result;
            ++result;
        }
        BS_UNREACHABLE();
    }

private:
    template<class Fn>
    constexpr string_view strip_impl(Fn match_fn) const noexcept {
        const auto* left_it = data();
        for (;; ++left_it) {
            if (left_it == data() + size() || !match_fn(*left_it)) break;
        }
        const auto* right_it = data() + size() - 1;
        if (left_it != data() + size()) {
            for (;; --right_it) {
                if (!match_fn(*right_it) || right_it == left_it) break;
            }
        }
        return string_view(left_it, right_it + 1);
    }
    template<class Fn>
    constexpr string_view lstrip_impl(Fn match_fn) const noexcept {
        for (const auto* it = data();; ++it) {
            if (it == data() + size() || !match_fn(*it)) {
                return string_view(it, data() + size());
            }
        }
        BS_UNREACHABLE();
    }
    template<class Fn>
    constexpr string_view rstrip_impl(Fn match_fn) const noexcept {
        for (const auto* it = data() + size() - 1;; --it) {
            if (!match_fn(*it) || it == data()) {
                return string_view(data(), it + 1);
            }
        }
        BS_UNREACHABLE();
    }

    template<class StripFn>
    constexpr string_view strip_impl2(StripFn strip_fn, const value_type strip_ch) const noexcept {
        return strip_fn([&](auto ch) { return ch == strip_ch; });
    }
    template<class StripFn>
    constexpr string_view strip_impl2(StripFn strip_fn, const string_view chs) const noexcept {
        detail::char_bitmap<value_type> bitmap;
        bitmap.mark(chs.data(), chs.data() + chs.size());
        if (!bitmap.mark(chs.data(), chs.data() + chs.size())) {
            return strip_fn([&](auto ch) {
                return traits_type::find(chs.data(), chs.size(), ch);
            });
        }

        return strip_fn([&](auto ch) {
            return bitmap.match(ch);
        });
    }
    template<class StripFn>
    constexpr string_view strip_impl2(StripFn strip_fn) const noexcept {
        constexpr string_view strip_chs = "\t\n\v\f\r ";
        detail::char_bitmap<value_type> bitmap;

        if (!bitmap.mark(strip_chs.data(), strip_chs.data() + strip_chs.size())) {
            BS_UNREACHABLE();
        }

        return strip_fn([&](auto ch) { return bitmap.match(ch); });
    }


public:

    constexpr string_view strip(const value_type strip_ch) const noexcept {
        return strip_impl2([&](auto f) { return strip_impl(f); }, strip_ch);
    }
    constexpr string_view strip(const string_view chs) const noexcept {
        return strip_impl2([&](auto f) { return strip_impl(f); }, chs);
    }
    constexpr string_view strip() const noexcept {
        return strip_impl2([&](auto f) { return strip_impl(f); });
    }

    constexpr string_view lstrip(const value_type strip_ch) const noexcept {
        return strip_impl2([&](auto f) { return lstrip_impl(f); }, strip_ch);
    }
    constexpr string_view lstrip(const string_view chs) const noexcept {
        return strip_impl2([&](auto f) { return lstrip_impl(f); }, chs);
    }
    constexpr string_view lstrip() const noexcept {
        return strip_impl2([&](auto f) { return lstrip_impl(f); });
    }

    constexpr string_view rstrip(const value_type strip_ch) const noexcept {
        return strip_impl2([&](auto f) { return rstrip_impl(f); }, strip_ch);
    }
    constexpr string_view rstrip(const string_view chs) const noexcept {
        return strip_impl2([&](auto f) { return rstrip_impl(f); }, chs);
    }
    constexpr string_view rstrip() const noexcept {
        return strip_impl2([&](auto f) { return rstrip_impl(f); });
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

#ifndef BS_DONT_INCLUDE_STRING
    constexpr operator std::basic_string_view<value_type, traits_type>() const noexcept {
        return std::basic_string_view<value_type, traits_type>(data(), size());
    }
#endif

private:
    const_pointer string_data;
    size_type string_size;
};

using wstring_view = string_view<char_traits<wchar_t>>;
using u16string_view = string_view<char_traits<char16_t>>;
using u32string_view = string_view<char_traits<char32_t>>;

#if __cplusplus >= 202002L
using u8string_view = string_view<char_traits<char8_t>>;
#endif

template<class Traits>
struct splited_string {
public:
    using separator_type = string_view<Traits>;
    using string_type = string_view<Traits>;
    using size_type = typename Traits::size_type;

    constexpr splited_string(const string_type str, const separator_type sep) noexcept
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
        const separator_type separator;
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
    const string_type string;
    const separator_type separator;
};

template<class Traits>
constexpr std::size_t strlen(const string_view<Traits> str) noexcept {
    return str.size();
}

template<class Traits>
constexpr void strcopy(typename Traits::char_type* const dest, const std::size_t dest_size, const string_view<Traits> src) noexcept {
    bs::strcopy(dest, dest_size, src.data(), src.size());
}

template<class Traits>
constexpr int strcomp(const string_view<Traits> left, const string_view<Traits> right) noexcept {
    return bs::strcomp(left.data(), left.size(), right.data(), right.size());
}

template<class Traits>
[[nodiscard]] constexpr bool empty(const string_view<Traits> str) noexcept {
    return str.empty();
}

template<class Traits>
[[nodiscard]] constexpr auto strfind(
    const string_view<Traits> str, const typename Traits::char_type ch
) noexcept -> const typename Traits::char_type* {
    return bs::strfind(str.data(), str.size(), ch);
}

template<class Traits>
constexpr auto strrfind(
    const string_view<Traits> str, const typename Traits::char_type ch
) noexcept -> const typename Traits::char_type* {
    return bs::strrfind(str.data(), str.size(), ch);
}

template<class Traits>
constexpr auto cstr(const string_view<Traits> str) noexcept
    -> const typename Traits::char_type* {
    return str.data();
}

template<class Traits>
constexpr auto strfind(const string_view<Traits> haystack, const string_view<detail::type_identity_t<Traits>> needle) noexcept
    -> const typename Traits::char_type* {
    return bs::strfind(haystack.data(), haystack.size(), needle.data(), needle.size());
}

template<class Traits>
constexpr auto strrfind(const string_view<Traits> haystack, const string_view<detail::type_identity_t<Traits>> needle) noexcept
    -> const typename Traits::char_type* {
    return bs::strrfind(haystack.data(), haystack.size(), needle.data(), needle.size());
}

template<class Traits>
constexpr void strmove(typename Traits::char_type* const dest, const std::size_t dest_size, const string_view<Traits> src) noexcept {
    bs::strmove(dest, dest_size, src.data(), src.size());
}

inline namespace literals {
    constexpr bs::string_view<> operator ""_sv(const char* const str, const std::size_t len) noexcept {
        return bs::string_view(str, len);
    }
    constexpr bs::wstring_view operator ""_sv(const wchar_t* const str, const std::size_t len) noexcept {
        return bs::wstring_view(str, len);
    }
    constexpr bs::u16string_view operator ""_sv(const char16_t* const str, const std::size_t len) noexcept {
        return bs::u16string_view(str, len);
    }
    constexpr bs::u32string_view operator ""_sv(const char32_t* const str, const std::size_t len) noexcept {
        return bs::u32string_view(str, len);
    }
#if __cplusplus >= 202002L
    constexpr bs::u8string_view operator ""_sv(const char8_t* const str, const std::size_t len) noexcept {
        return bs::u8string_view(str, len);
    }
#endif
}

}
