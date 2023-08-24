#pragma once

#include <string>
#include <iterator>
#include <type_traits>
#include <memory>
#include <cstring>

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
    constexpr const T& min(const T& x, const T& y) noexcept {
        return y < x ? y : x;
    }

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

template<class T>
constexpr std::size_t strlen(const T* const str) noexcept {
    static_assert(is_character<T>);
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
    bs::strcopy(dest, src, detail::min(dest_size, count));
}

template<class T>
constexpr int strcomp(const T* const left, const T* const right, const std::size_t count) noexcept {
#if BS_HAS_BUILTIN(__builtin_wmemcmp) || defined(_MSC_VER)
    if constexpr (std::is_same_v<T, wchar_t>) {
        return __builtin_wmemcmp(left, right, count);
    } else
#endif
#if (BS_HAS_BUILTIN(__builtin_memcmp) || defined(_MSC_VER)) && false
    return __builtin_memcmp(left, right, count * sizeof(T));
#else
    if (detail::is_constant_evaluated()) {
        for (std::size_t i = 0; i < count; ++i) {
            if (left[i] < right[i]) return -1;
            if (left[i] > right[i]) return 1;
        }
        return 0;
    } else {
        if constexpr (std::is_same_v<T, wchar_t>) {
            return std::wmemcmp(left, right, count);
        } else {
            return std::memcmp(left, right, count * sizeof(T));
        }
    }
#endif
}

template<class T>
constexpr int strcomp(const T* const left, const std::size_t left_len, const T* const right, const std::size_t right_len) noexcept {
    if (left_len > right_len) return 1;
    if (left_len < right_len) return -1;
    return bs::strcomp(left, right, left_len);
}

template<class T>
[[nodiscard]] constexpr bool empty(const T* const str) noexcept {
    return str[0] == '\0';
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
    struct type_identity { using type = T; };

    template<class T>
    using type_identity_t = typename type_identity<T>::type;

    template<class T>
    class char_bitmap {
    public:
        constexpr bool mark(const T* first, const T* const last) noexcept {
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
struct char_traits : std::char_traits<T> {
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
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

    constexpr size_type find(const string_view str, const size_type start = 0) const noexcept {
        return this->find(str, start, size());
    }
    constexpr size_type find(const string_view str, const size_type start, const size_type end) const noexcept {
        BS_VERIFY(end <= size(), "end is out of range");
        if (str.size() + start > end) return end;
        if (str.empty()) return start;

        const auto match_max = data() + (end - str.size()) + 1;
        for (auto* match_try = data() + start;; ++match_try) {
            match_try = traits_type::find(match_try, static_cast<size_type>(match_max - match_try), str[0]);
            if (match_try == nullptr) return end;
            if (traits_type::compare(match_try, str.data(), str.size()) == 0) {
                return static_cast<size_type>(match_try - data());
            }
        }
        BS_UNREACHABLE();
    }

    constexpr size_type find(const value_type ch, const size_type start = 0) const noexcept {
        return this->find(ch, start, size());
    }
    constexpr size_type find(const value_type ch, const size_type start, const size_type end) const noexcept {
        BS_VERIFY(end <= size(), "end is out of range");
        if (start >= end) return end;
        const auto match_result = traits_type::find(data() + start, end - start, ch);
        return match_result == nullptr ? end : static_cast<size_type>(match_result - data());
    }

    constexpr size_type rfind(const value_type ch) const noexcept {
        return this->rfind(ch, size(), 0);
    }
    constexpr size_type rfind(const value_type ch, const size_type start, const size_type end = 0) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        if (start <= end) return end;
        for (const auto* it = data() + start - 1;; --it) {
            if (traits_type::eq(*it, ch)) {
                return static_cast<size_type>(it - data());
            }
            if (it == data() + end) return end;
        }
        BS_UNREACHABLE();
    }

    constexpr size_type rfind(const string_view str) const noexcept {
        return this->rfind(str, size(), 0);
    }
    constexpr size_type rfind(const string_view str, const size_type start, const size_type end = 0) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        if (str.size() + end > start) return end;
        if (str.empty()) return start;
        for (auto* match_try = data() + (start - str.size());; --match_try) {
            if (traits_type::compare(match_try, str.data(), str.size()) == 0) {
                return static_cast<size_type>(match_try - data());
            }
            if (match_try == data() + end) return end;
        }
        BS_UNREACHABLE();
    }

    constexpr splited_string<traits_type> split(const string_view separator) const noexcept {
        return splited_string<traits_type>(*this, separator);
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

private:
    const_pointer string_data;
    size_type string_size;
};

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
constexpr void strcopy(typename Traits::char_type* const dest, const string_view<Traits> src) noexcept {
    bs::strcopy(dest, src.data(), src.size());
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

}
