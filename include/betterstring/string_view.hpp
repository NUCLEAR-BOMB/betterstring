
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <iterator>
#include <type_traits>
#include <memory>
#include <cstring>
#include <algorithm>
#include <stdexcept>

#include <betterstring/detail/preprocessor.hpp>
#include <betterstring/detail/integer_cmps.hpp>
#include <betterstring/detail/ranges_traits.hpp>
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

struct slice {
    using index_type = std::ptrdiff_t;

    constexpr slice(const index_type stop_) noexcept
        : start(0), stop(stop_) {}
    constexpr slice(const index_type start_, const index_type stop_) noexcept
        : start(start_), stop(stop_) {}

    index_type start;
    index_type stop;
};

template<class Traits>
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
    using find_res = bs::find_result<const value_type, size_type>;
    using sfind_res = bs::find_result<const value_type, std::make_signed_t<size_type>>;
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

    template<class Range, std::enable_if_t<
        !std::is_same_v<detail::remove_cvref_t<Range>, string_viewt>
        && !std::is_convertible_v<Range, const value_type*>
        && !detail::has_conversion_operator<detail::remove_cvref_t<Range>&, string_viewt>
        && detail::ranges::is_contiguous_range<Range>
    , int> = 0>
    constexpr string_viewt(BS_LIFETIMEBOUND Range&& range) noexcept(noexcept(detail::ranges::data(range), detail::ranges::size(range)))
        : string_data{detail::ranges::data(range)}
        , string_size{detail::ranges::size(range)} {}

    [[nodiscard]] static constexpr string_viewt from_c_string(const const_pointer c_str) noexcept {
        BS_VERIFY(c_str != nullptr, "c_str is null pointer");
        return string_viewt{c_str, traits_type::length(c_str)};
    }

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
        return data()[index < 0 ? index + Int(size()) : index];
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
    constexpr size_type size_bytes() const noexcept { return string_size * sizeof(value_type); }
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
        const size_type start_index = start < 0 ? start + Int1(size()) : start;
        const size_type finish_index = finish < 0 ? finish + Int2(size()) : finish;
        return string_viewt{data() + start_index, finish_index - start_index};
    }
    template<class Int1, class SliceEnd = slice_end_tag, std::enable_if_t<
        std::is_integral_v<Int1> && std::is_same_v<SliceEnd, slice_end_tag>
    , int> = 0>
    constexpr string_viewt operator()(const Int1 start, const SliceEnd) const noexcept {
        BS_VERIFY((start + Int1(size())) >= 0 && start <= Int1(size()), "start index is out of range");
        const size_type start_index = start < 0 ? start + Int1(size()) : start;
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
        if (size() < str.size()) { return false; }
        return traits_type::compare(data(), str.data(), str.size()) == 0;
    }
    constexpr bool starts_with(const value_type ch) const noexcept {
        return !empty() && traits_type::eq(front(), ch);
    }

    constexpr bool ends_with(const string_viewt str) const noexcept {
        if (size() < str.size()) { return false; }
        return traits_type::compare(data() + (size() - str.size()), str.data(), str.size()) == 0;
    }
    constexpr bool ends_with(const value_type ch) const noexcept {
        return !empty() && traits_type::eq(back(), ch);
    }

    constexpr find_res find(const value_type ch) const noexcept {
        return { data(), size(), traits_type::find(data(), size(), ch) };
    }
    constexpr find_res find(const string_viewt str) const noexcept {
        return { data(), size(), traits_type::findstr(data(), size(), str.data(), str.size()) };
    }
    constexpr find_res find(const value_type ch, const size_type start) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        return { data(), size(), traits_type::find(data() + start, size() - start, ch) };
    }
    constexpr find_res find(const string_viewt str, const size_type start) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        return { data(), size(), traits_type::findstr(data() + start, size() - start, str.data(), str.size()) };
    }
    constexpr find_res find(const value_type ch, const const_pointer start) const noexcept {
        BS_VERIFY(start >= data() && start <= data_end(), "start is out of range");
        return { data(), size(), traits_type::find(start, size() - (start - data()), ch) };
    }
    constexpr find_res find(const string_viewt str, const const_pointer start) const noexcept {
        BS_VERIFY(start >= data() && start <= data_end(), "start is out of range");
        return { data(), size(), traits_type::findstr(start, size() - (start - data()), str.data(), str.size()) };
    }

    constexpr sfind_res rfind(const value_type ch) const noexcept {
        return { data(), -1, traits_type::rfind(data(), size(), ch),};
    }
    constexpr sfind_res rfind(const string_viewt str) const noexcept {
        return { data(), -1, traits_type::rfindstr(data(), size(), str.data(), str.size()) };
    }
    constexpr sfind_res rfind(const value_type ch, const size_type start) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        return { data(), -1, traits_type::rfind(data(), start, ch) };
    }
    constexpr sfind_res rfind(const string_viewt str, const size_type start) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        return { data(), -1, traits_type::rfindstr(data(), start, str.data(), str.size()) };
    }
    constexpr sfind_res rfind(const value_type ch, const const_pointer start) const noexcept {
        BS_VERIFY(start >= data() && start <= data_end(), "start is out of range");
        return { data(), -1, traits_type::rfind(data(), (start - data()), ch) };
    }
    constexpr sfind_res rfind(const string_viewt str, const const_pointer start) const noexcept {
        BS_VERIFY(start >= data() && start <= data_end(), "start is out of range");
        return { data(), -1, traits_type::rfindstr(data(), (start - data()), str.data(), str.size())};
    }

    constexpr find_res find_not(const value_type ch) const noexcept {
        return { data(), size(), traits_type::find_not(data(), size(), ch) };
    }
    constexpr find_res find_not(const string_viewt str) const noexcept {
        return { data(), size(), traits_type::findstr_not(data(), size(), str.data(), str.size()) };
    }
    constexpr find_res find_not(const value_type ch, const size_type start) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        return { data(), size(), traits_type::find_not(data() + start, size() - start, ch) };
    }
    constexpr find_res find_not(const string_viewt str, const size_type start) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        return { data(), size(), traits_type::findstr_not(data() + start, size() - start, str.data(), str.size()) };
    }
    constexpr find_res find_not(const value_type ch, const const_pointer start) const noexcept {
        BS_VERIFY(start >= data() && start <= data_end(), "start is out of range");
        return { data(), size(), traits_type::find_not(start, size() - (start - data()), ch) };
    }
    constexpr find_res find_not(const string_viewt str, const const_pointer start) const noexcept {
        BS_VERIFY(start >= data() && start <= data_end(), "start is out of range");
        return { data(), size(), traits_type::findstr_not(start, size() - (start - data()), str.data(), str.size()) };
    }

    constexpr sfind_res rfind_not(const value_type ch) const noexcept {
        return { data(), -1, traits_type::rfind_not(data(), size(), ch),};
    }
    constexpr sfind_res rfind_not(const string_viewt str) const noexcept {
        return { data(), -1, traits_type::rfindstr_not(data(), size(), str.data(), str.size()) };
    }
    constexpr sfind_res rfind_not(const value_type ch, const size_type start) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        return { data(), -1, traits_type::rfind_not(data(), start, ch) };
    }
    constexpr sfind_res rfind_not(const string_viewt str, const size_type start) const noexcept {
        BS_VERIFY(start <= size(), "start is out of range");
        return { data(), -1, traits_type::rfindstr_not(data(), start, str.data(), str.size()) };
    }
    constexpr sfind_res rfind_not(const value_type ch, const const_pointer start) const noexcept {
        BS_VERIFY(start >= data() && start <= data_end(), "start is out of range");
        return { data(), -1, traits_type::rfind_not(data(), (start - data()), ch) };
    }
    constexpr sfind_res rfind_not(const string_viewt str, const const_pointer start) const noexcept {
        BS_VERIFY(start >= data() && start <= data_end(), "start is out of range");
        return { data(), -1, traits_type::rfindstr_not(data(), (start - data()), str.data(), str.size())};
    }

    constexpr find_res find_first_of(const string_viewt str) const noexcept {
        return { data(), size(), traits_type::first_of(data(), size(), str.data(), str.size()) };
    }
    constexpr sfind_res find_last_of(const string_viewt str) const noexcept {
        return { data(), -1, traits_type::last_of(data(), size(), str.data(), str.size()) };
    }
    constexpr find_res find_first_not_of(const string_viewt str) const noexcept {
        return { data(), size(), traits_type::first_not_of(data(), size(), str.data(), str.size()) };
    }
    constexpr sfind_res find_last_not_of(const string_viewt str) const noexcept {
        return { data(), -1, traits_type::last_not_of(data(), size(), str.data(), str.size()) };
    }

    constexpr bool contains(const value_type ch) const noexcept {
        return traits_type::find(data(), size(), ch) != nullptr;
    }
    constexpr bool contains(const string_viewt str) const noexcept {
        return traits_type::findstr(data(), size(), str.data(), str.size()) != nullptr;
    }
    constexpr bool contains_any_of(const string_viewt chs) const noexcept {
        return traits_type::first_of(data(), size(), chs.data(), chs.size()) != nullptr;
    }

    constexpr bool rcontains(const value_type ch) const noexcept {
        return traits_type::rfind(data(), size(), ch) != nullptr;
    }
    constexpr bool rcontains(const string_viewt str) const noexcept {
        return traits_type::rfindstr(data(), size(), str.data(), str.size()) != nullptr;
    }
    constexpr bool rcontains_any_of(const string_viewt chs) const noexcept {
        return traits_type::last_of(data(), size(), chs.data(), chs.size()) != nullptr;
    }

    constexpr splited_string<string_viewt, string_viewt> split(const string_viewt separator) const noexcept {
        return splited_string<string_viewt, string_viewt>(*this, separator);
    }
    constexpr splited_string<string_viewt, value_type> split(const value_type character) const noexcept {
        return splited_string<string_viewt, value_type>(*this, character);
    }
    constexpr reverse_splited_string<string_viewt, string_viewt> rsplit(const string_viewt separator) const noexcept {
        return reverse_splited_string<string_viewt, string_viewt>{*this, separator};
    }
    constexpr reverse_splited_string<string_viewt, value_type> rsplit(const value_type separator) const noexcept {
        return reverse_splited_string<string_viewt, value_type>{*this, separator};
    }

    constexpr size_type count(const value_type ch) const noexcept {
        return traits_type::count(data(), size(), ch);
    }
    constexpr size_type count(const string_viewt str) const noexcept {
        return traits_type::countstr(data(), size(), str.data(), str.size());
    }
    constexpr size_type count_any_of(const string_viewt chs) const noexcept {
        return traits_type::count_any_of(data(), size(), chs.data(), chs.size());
    }

    constexpr string_viewt strip(const value_type ch) const noexcept {
        const auto first = traits_type::find_not(data(), size(), ch);
        if (first == nullptr) { return string_viewt{}; }
        const auto last = traits_type::rfind_not(data(), size(), ch);
        BS_ASSUME(last != nullptr);
        return string_viewt{first, last + 1};
    }
    BS_FLATTEN
    constexpr string_viewt strip(const string_viewt chs) const noexcept {
        // hopefully the compiler is smart enough to merge two bitmaps into one
        const auto first = traits_type::first_not_of(data(), size(), chs.data(), chs.size());
        if (first == nullptr) { return string_viewt{}; }
        const auto last = traits_type::last_not_of(data(), size(), chs.data(), chs.size());
        BS_ASSUME(last != nullptr);
        return string_viewt{first, last + 1};
    }

    constexpr string_viewt strip_left(const value_type ch) const noexcept {
        const auto first = traits_type::find_not(data(), size(), ch);
        return string_viewt{first, data() + size()};
    }
    constexpr string_viewt strip_left(const string_viewt chs) const noexcept {
        const auto first = traits_type::first_not_of(data(), size(), chs.data(), chs.size());
        return string_viewt{first, data() + size()};
    }

    constexpr string_viewt strip_right(const value_type ch) const noexcept {
        const auto last = traits_type::rfind_not(data(), size(), ch);
        return string_viewt{data(), last + 1};
    }
    constexpr string_viewt strip_right(const string_viewt chs) const noexcept {
        const auto last = traits_type::last_not_of(data(), size(), chs.data(), chs.size());
        return string_viewt{data(), last + 1};
    }

    constexpr string_viewt strip_first(const value_type ch) const noexcept {
        if (!starts_with(ch)) { return *this; }
        return string_viewt{data() + 1, size() - 1};
    }
    constexpr string_viewt strip_first(const string_viewt str) const noexcept {
        if (!starts_with(str)) { return *this; }
        return string_viewt{data() + str.size(), size() - str.size()};
    }

    constexpr string_viewt strip_last(const value_type ch) const noexcept {
        if (!ends_with(ch)) { return *this; }
        return string_viewt{data(), size() - 1};
    }
    constexpr string_viewt strip_last(const string_viewt str) const noexcept {
        if (!ends_with(str)) { return *this; }
        return string_viewt{data(), size() - str.size()};
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

    friend constexpr bool operator==(const string_viewt left, const string_viewt right) noexcept {
        if (left.size() != right.size()) { return false; }
        return Traits::compare(left.data(), right.data(), left.size()) == 0;
    }
    friend constexpr bool operator!=(const string_viewt left, const string_viewt right) noexcept {
        if (left.size() != right.size()) { return true; }
        return Traits::compare(left.data(), right.data(), left.size()) != 0;
    }
    friend constexpr bool operator>(const string_viewt left, const string_viewt right) noexcept {
        if (left.size() != right.size()) { return left.size() > right.size(); }
        return Traits::compare(left.data(), right.data(), left.size()) > 0;
    }
    friend constexpr bool operator>=(const string_viewt left, const string_viewt right) noexcept {
        if (left.size() != right.size()) { return left.size() > right.size(); }
        return Traits::compare(left.data(), right.data(), left.size()) >= 0;
    }
    friend constexpr bool operator<(const string_viewt left, const string_viewt right) noexcept {
        if (left.size() != right.size()) { return left.size() < right.size(); }
        return Traits::compare(left.data(), right.data(), left.size()) < 0;
    }
    friend constexpr bool operator<=(const string_viewt left, const string_viewt right) noexcept {
        if (left.size() != right.size()) { return left.size() < right.size(); }
        return Traits::compare(left.data(), right.data(), left.size()) <= 0;
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
