#pragma once

#include <iterator>
#include <type_traits>
#include <memory>
#include <cstring>
#include <algorithm>
#include <stdexcept>

#include <betterstring/detail/preprocessor.hpp>
#include <betterstring/detail/integer_cmps.hpp>
#include <betterstring/functions.hpp>
#include <betterstring/splited_string.hpp>
#include <betterstring/char_traits.hpp>
#include <betterstring/type_traits.hpp>
#include <betterstring/find_result.hpp>

#if BS_COMP_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wsign-conversion"
#endif

namespace bs
{

namespace detail {
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

struct slice {
    using index_type = std::ptrdiff_t;

    constexpr slice(const index_type stop_) noexcept
        : start(0), stop(stop_) {}
    constexpr slice(const index_type start_, const index_type stop_) noexcept
        : start(start_), stop(stop_) {}

    index_type start;
    index_type stop;
};

template<class Traits = char_traits<char>>
class string_viewt {
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

private:
    using signed_size_type = std::make_signed_t<size_type>;
public:

    constexpr string_viewt() noexcept : string_data(nullptr), string_size(0) {}

    constexpr string_viewt(const string_viewt&) noexcept = default;

    constexpr string_viewt(const const_pointer str, const size_type count)
        : string_data(str), string_size(count) {
        BS_VERIFY(count == 0 || str != nullptr, "null pointer with non-zero size");
    }

    template<std::size_t N>
    constexpr string_viewt(const value_type(&array)[N]) noexcept
        : string_data(array), string_size(N - 1) { // remove null terminator
        static_assert(N != 0, "given non-null terminated array");
    }

    template<class Begin, class End = Begin, std::enable_if_t<
        !std::is_convertible_v<End, size_type>
    , int> = 0>
    constexpr string_viewt(Begin first, End last) noexcept(noexcept(last - first))
        : string_data(detail::to_address(first))
        , string_size(static_cast<size_type>(last - first)) {}

    constexpr string_viewt(std::nullptr_t) = delete;

    constexpr string_viewt& operator=(const string_viewt&) noexcept = default;

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

    template<class Int, std::enable_if_t<std::is_integral_v<Int>, int> = 0>
    constexpr const_reference operator[](const Int index) const noexcept {
        BS_VERIFY((index + Int(size())) >= 0 && index < Int(size()), "index is out of range");
        return data()[index < 0 ? index + size() : index];
    }
    template<class Int, std::enable_if_t<std::is_integral_v<Int>, int> = 0>
    constexpr reference operator[](const Int index) noexcept {
        BS_VERIFY((index + Int(size())) >= 0 && index < Int(size()), "index is out of range");
        return data()[index < 0 ? index + size() : index];
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
    BS_CONST_FN constexpr const_pointer data_end() const noexcept { return data() + size(); }

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
    struct slice_end_tag {};
public:

    template<class Int1, class Int2, std::enable_if_t<
        std::is_integral_v<Int1> && std::is_integral_v<Int2>
    , int> = 0>
    constexpr string_viewt operator()(const Int1 start, const Int2 finish) const noexcept {
        BS_VERIFY((start + Int1(size())) >= 0 && start <= Int1(size()), "start index is out of range");
        BS_VERIFY((finish + Int2(size())) >= 0 && finish <= Int2(size()), "finish index is out of range");
        const size_type start_index = start < 0 ? start + size() : start;
        const size_type finish_index = finish < 0 ? finish + size() : finish;
        return string_viewt{data() + start_index, finish_index - start_index};
    }
    template<class Int1, class SliceEnd = slice_end_tag, std::enable_if_t<
        std::is_integral_v<Int1> && std::is_same_v<SliceEnd, slice_end_tag>
    , int> = 0>
    constexpr string_viewt operator()(const Int1 start, const SliceEnd) const noexcept {
        BS_VERIFY((start + Int1(size())) >= 0 && start <= Int1(size()), "start index is out of range");
        const size_type start_index = start < 0 ? start + size() : start;
        return string_viewt{data() + start_index, size() - start_index};
    }
    constexpr string_viewt operator[](const bs::slice slice) const noexcept {
        using sidx_t = std::ptrdiff_t;
        BS_VERIFY((slice.start + sidx_t(size())) >= 0 && slice.start <= sidx_t(size()), "slice start index is out of range");
        BS_VERIFY((slice.stop + sidx_t(size())) >= 0 && slice.stop <= sidx_t(size()), "slice stop index is out of range");
        const size_type start_index = slice.start < 0 ? slice.start + size() : slice.start;
        const size_type finish_index = slice.stop < 0 ? slice.stop + size() : slice.stop;
        return string_viewt{data() + start_index, finish_index - start_index};
    }

    constexpr string_viewt substr(const size_type position) const noexcept {
        BS_VERIFY(position <= size(), "the start position of the substring exceeds the length of the string");
        return (*this)(position, size());
    }
    constexpr string_viewt substr(const size_type position, const size_type count) const noexcept {
        BS_VERIFY(position <= size(), "the start position of the substring exceeds the length of the string");
        return (*this)(position, position + count);
    }

    constexpr bool starts_with(const string_viewt str) const noexcept {
        if (size() < str.size()) return false;
        return traits_type::compare(data(), str.data(), str.size()) == 0;
    }
    constexpr bool starts_with(const value_type ch) const noexcept {
        return !empty() && traits_type::eq(front(), ch);
    }

    constexpr bool ends_with(const string_viewt str) const noexcept {
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

    constexpr this_find_result find(const value_type ch) const noexcept {
        return make_find_r(traits_type::find(data(), size(), ch));
    }
    constexpr this_find_result find(const string_viewt str) const noexcept {
        return make_find_r(traits_type::findstr(data(), size(), str.data(), str.size()));
    }
    constexpr this_find_result find(const value_type ch, const size_type start) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        return make_find_r(traits_type::find(data() + start, size() - start, ch));
    }
    constexpr this_find_result find(const string_viewt str, const size_type start) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        return make_find_r(traits_type::findstr(data() + start, size() - start, str.data(), str.size()));
    }
    constexpr this_find_result find(const value_type ch, const const_pointer start) const noexcept {
        BS_VERIFY(start >= data() && start <= data_end(), "start is out of range");
        return make_find_r(traits_type::find(start, size() - (start - data()), ch));
    }
    constexpr this_find_result find(const string_viewt str, const const_pointer start) const noexcept {
        BS_VERIFY(start >= data() && start <= data_end(), "start is out of range");
        return make_find_r(traits_type::findstr(start, size() - (start - data()), str.data(), str.size()));
    }

    constexpr this_find_result rfind(const value_type ch) const noexcept {
        return make_find_r(traits_type::rfind(data(), size(), ch), 0);
    }
    constexpr this_find_result rfind(const string_viewt str) const noexcept {
        return make_find_r(traits_type::rfindstr(data(), size(), str.data(), str.size()), 0);
    }
    constexpr this_find_result rfind(const value_type ch, const size_type start) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        return make_find_r(traits_type::rfind(data(), start, ch), 0);
    }
    constexpr this_find_result rfind(const string_viewt str, const size_type start) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        return make_find_r(traits_type::rfindstr(data(), start, str.data(), str.size()), 0);
    }
    constexpr this_find_result rfind(const value_type ch, const const_pointer start) const noexcept {
        BS_VERIFY(start >= data() && start <= data_end(), "start is out of range");
        return make_find_r(traits_type::rfind(data(), (start - data()), ch), 0);
    }
    constexpr this_find_result rfind(const string_viewt str, const const_pointer start) const noexcept {
        BS_VERIFY(start >= data() && start <= data_end(), "start is out of range");
        return make_find_r(traits_type::rfindstr(start, (start - data()), str.data(), str.size()), 0);
    }

    constexpr this_find_result find_first_of(const value_type ch) const noexcept {
        return this->find(ch);
    }
    constexpr this_find_result find_first_of(const string_viewt str) const noexcept {
        return make_find_r(traits_type::first_of(data(), size(), str.data(), str.size()));
    }

    constexpr this_find_result find_last_of(const value_type ch) const noexcept {
        return this->rfind(ch);
    }
    constexpr this_find_result find_last_of(const string_viewt str) const noexcept {
        detail::char_bitmap<value_type> bitmap;
        if (!bitmap.mark(str.data(), str.data() + str.size())) {
            for (auto match_try = data() + size() - 1; match_try != data() - 1; --match_try) {
                if (traits_type::find(str.data(), str.size(), *match_try) != nullptr) {
                    return make_find_r(match_try);
                }
            }
            return make_find_r(nullptr);
        }
        for (size_type i = size(); i != 0; --i) {
            if (bitmap.match(data()[i - 1])) return make_find_r(i - 1);
        }
        return make_find_r(nullptr);
    }

    constexpr bool contains(const value_type ch) const noexcept {
        return traits_type::find(data(), size(), ch) != nullptr;
    }
    constexpr bool contains(const string_viewt str) const noexcept {
        return traits_type::findstr(data(), size(), str.data(), str.size()) != nullptr;
    }

    constexpr splited_string<string_viewt, string_viewt> split(const string_viewt separator) const noexcept {
        return splited_string<string_viewt, string_viewt>(*this, separator);
    }
    constexpr splited_string<string_viewt, value_type> split(const value_type character) const noexcept {
        return splited_string<string_viewt, value_type>(*this, character);
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
    constexpr size_type count(const string_viewt str) const noexcept {
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
    constexpr string_viewt strip_impl(Fn match_fn) const noexcept {
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
        return string_viewt{left_it, right_it + 1};
    }
    template<class Fn>
    constexpr string_viewt lstrip_impl(Fn match_fn) const noexcept {
        for (const auto* it = data();; ++it) {
            if (it == data() + size() || !match_fn(*it)) {
                return string_viewt{it, data() + size()};
            }
        }
        BS_UNREACHABLE();
    }
    template<class Fn>
    constexpr string_viewt rstrip_impl(Fn match_fn) const noexcept {
        for (const auto* it = data() + size() - 1;; --it) {
            if (!match_fn(*it) || it == data()) {
                return string_viewt(data(), it + 1);
            }
        }
        BS_UNREACHABLE();
    }

    template<class StripFn>
    constexpr string_viewt strip_impl2(StripFn strip_fn, const value_type strip_ch) const noexcept {
        return strip_fn([&](auto ch) { return ch == strip_ch; });
    }
    template<class StripFn>
    constexpr string_viewt strip_impl2(StripFn strip_fn, const string_viewt chs) const noexcept {
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
    constexpr string_viewt strip_impl2(StripFn strip_fn) const noexcept {
        constexpr string_viewt strip_chs{"\t\n\v\f\r ", 6};
        detail::char_bitmap<value_type> bitmap;

        if (!bitmap.mark(strip_chs.data(), strip_chs.data() + strip_chs.size())) {
            BS_UNREACHABLE();
        }

        return strip_fn([&](auto ch) { return bitmap.match(ch); });
    }


public:

    constexpr string_viewt strip(const value_type strip_ch) const noexcept {
        return strip_impl2([&](auto f) { return strip_impl(f); }, strip_ch);
    }
    constexpr string_viewt strip(const string_viewt chs) const noexcept {
        return strip_impl2([&](auto f) { return strip_impl(f); }, chs);
    }
    constexpr string_viewt strip() const noexcept {
        return strip_impl2([&](auto f) { return strip_impl(f); });
    }

    constexpr string_viewt lstrip(const value_type strip_ch) const noexcept {
        return strip_impl2([&](auto f) { return lstrip_impl(f); }, strip_ch);
    }
    constexpr string_viewt lstrip(const string_viewt chs) const noexcept {
        return strip_impl2([&](auto f) { return lstrip_impl(f); }, chs);
    }
    constexpr string_viewt lstrip() const noexcept {
        return strip_impl2([&](auto f) { return lstrip_impl(f); });
    }

    constexpr string_viewt rstrip(const value_type strip_ch) const noexcept {
        return strip_impl2([&](auto f) { return rstrip_impl(f); }, strip_ch);
    }
    constexpr string_viewt rstrip(const string_viewt chs) const noexcept {
        return strip_impl2([&](auto f) { return rstrip_impl(f); }, chs);
    }
    constexpr string_viewt rstrip() const noexcept {
        return strip_impl2([&](auto f) { return rstrip_impl(f); });
    }

    constexpr string_viewt strip_first(const value_type ch) const noexcept {
        if (!starts_with(ch)) return *this;
        return string_viewt(data() + 1, size() - 1);
    }
    constexpr string_viewt strip_first(const string_viewt str) const noexcept {
        if (!starts_with(str)) return *this;
        return string_viewt(data() + str.size(), size() - str.size());
    }

    constexpr string_viewt strip_last(const value_type ch) const noexcept {
        if (!ends_with(ch)) return *this;
        return string_viewt(data(), size() - 1);
    }
    constexpr string_viewt strip_last(const string_viewt str) const noexcept {
        if (!ends_with(str)) return *this;
        return string_viewt(data(), size() - str.size());
    }

    template<class Predicate>
    constexpr bool all_of(Predicate pred) const {
        for (const value_type* it = data(); it != data_end(); ++it) {
            if (!bool(pred(*it))) {
                return false;
            }
        }
        return true;
    }
    template<class Predicate>
    constexpr bool any_of(Predicate pred) const {
        for (const value_type* it = data(); it != data_end(); ++it) {
            if (pred(*it)) {
                return true;
            }
        }
        return false;
    }
    template<class Predicate>
    constexpr bool none_of(Predicate pred) const {
        for (const value_type* it = data(); it != data_end(); ++it) {
            if (pred(*it)) {
                return false;
            }
        }
        return true;
    }

private:
    static constexpr int trait_cmp(const string_viewt l, const string_viewt r) noexcept {
        if (l.size() > r.size()) return 1;
        if (l.size() < r.size()) return -1;
        return Traits::compare(l.data(), r.data(), l.size());
    }
public:

    friend constexpr bool operator==(const string_viewt l, const string_viewt r) noexcept {
        return trait_cmp(l, r) == 0;
    }
    friend constexpr bool operator!=(const string_viewt l, const string_viewt r) noexcept {
        return trait_cmp(l, r) != 0;
    }
    friend constexpr bool operator>(const string_viewt l, const string_viewt r) noexcept {
        return trait_cmp(l, r) > 0;
    }
    friend constexpr bool operator>=(const string_viewt l, const string_viewt r) noexcept {
        return trait_cmp(l, r) >= 0;
    }
    friend constexpr bool operator<(const string_viewt l, const string_viewt r) noexcept {
        return trait_cmp(l, r) < 0;
    }
    friend constexpr bool operator<=(const string_viewt l, const string_viewt r) noexcept {
        return trait_cmp(l, r) <= 0;
    }

#ifndef BS_DONT_INCLUDE_STRING
    template<class Tr>
    constexpr operator std::basic_string_view<value_type, Tr>() const noexcept {
        return std::basic_string_view<value_type, Tr>(data(), size());
    }
    template<class Tr>
    explicit constexpr operator std::basic_string<value_type, Tr>() const noexcept {
        return std::basic_string<value_type, Tr>(data(), size());
    }
#endif

private:
    const_pointer string_data;
    size_type string_size;
};

using string_view = string_viewt<char_traits<char>>;
using wstring_view = string_viewt<char_traits<wchar_t>>;
using u16string_view = string_viewt<char_traits<char16_t>>;
using u32string_view = string_viewt<char_traits<char32_t>>;
#if BS_HAS_CHAR8_T
using u8string_view = string_view<char_traits<char8_t>>;
#endif

inline namespace literals {
    constexpr bs::string_view operator ""_sv(const char* const str, const std::size_t len) noexcept {
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
#if BS_HAS_CHAR8_T
    constexpr bs::u8string_view operator ""_sv(const char8_t* const str, const std::size_t len) noexcept {
        return bs::u8string_view(str, len);
    }
#endif
}

}

#if BS_COMP_CLANG
    #pragma clang diagnostic pop
#endif
