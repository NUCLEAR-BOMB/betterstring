
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <climits>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <optional>
#include <functional>

#include <betterstring/allocators.hpp>
#include <betterstring/char_traits.hpp>
#include <betterstring/type_traits.hpp>
#include <betterstring/string_view.hpp>
#include <betterstring/detail/preprocessor.hpp>
#include <betterstring/detail/reference_wrapper.hpp>

namespace bs {

namespace detail {
    template<class Char, class Size, Size extra_capacity>
    class string_representation {
        struct long_string {
            Char* data;
            Size size;
            Size capacity;
        };
        static constexpr std::size_t short_capacity = (sizeof(long_string) - sizeof(std::uint8_t)) / sizeof(Char);
        struct short_string {
            Char data[short_capacity];
            std::uint8_t size;
        };
        union string_rep {
            long_string long_str;
            short_string short_str;
        };
        string_rep rep;
    public:
        string_representation() = default;

        constexpr bool is_long() const noexcept {
            if (detail::is_constant_evaluated()) {
                return true;
            }
            return rep.short_str.size & (1 << 7);
        }
        constexpr void set_long_state() noexcept {
            rep.short_str.size |= (1 << 7);
        }
        constexpr void set_short_state() noexcept {
            rep.short_str.size &= ~(1 << 7);
        }

        static constexpr bool fits_in_sso(const Size size) noexcept {
            return size <= short_capacity;
        }

        constexpr Size get_short_size() const noexcept {
            BS_VERIFY(!is_long(), "string must be in short state when accessing short string size");
            return rep.short_str.size;
        }
        constexpr Size get_long_size() const noexcept {
            BS_VERIFY(is_long(), "string must be in long state when accessing long string size");
            return rep.long_str.size;
        }

        constexpr void set_short_size(const Size size) noexcept {
            BS_VERIFY(size <= short_capacity, "the size of short string exceeded short string capacity");
            rep.short_str.size = uint8_t(size);
        }
        constexpr void set_long_size(const Size size) noexcept {
            BS_VERIFY(size <= get_long_capacity(), "the size of long string exceeded long string capacity");
            rep.long_str.size = size;
        }

        constexpr Size get_short_capacity() const noexcept {
            BS_VERIFY(!is_long(), "string must be in short state when accessing short string capacity");
            return short_capacity;
        }
        constexpr Size get_long_capacity() const noexcept {
            BS_VERIFY(is_long(), "string must be in long state when accessing long string capacity");
            return rep.long_str.capacity & ~(uint64_t(1) << 63);
        }

        constexpr void set_long_capacity(Size cap) noexcept {
            BS_VERIFY(is_long(), "string must be in long state when setting capacity");
            cap |= (uint64_t(1) << 63); // set long string flag
            rep.long_str.capacity = cap;
        }

        constexpr Size get_size() const noexcept {
            return is_long() ? get_long_size() : get_short_size();
        }
        constexpr Size get_capacity() const noexcept {
            return is_long() ? get_long_capacity() : get_short_capacity();
        }

        constexpr void set_size(const Size size) noexcept {
            if (is_long()) {
                set_long_size(size);
            } else {
                set_short_size(size);
            }
        }

        constexpr void set_long_pointer(Char* const ptr) noexcept {
            BS_VERIFY(is_long(), "string must be in long state when setting long string pointer");
            rep.long_str.data = ptr;
        }

        constexpr Char* get_long_pointer() noexcept {
            BS_VERIFY(is_long(), "string must be in long state when accessing long string pointer");
            return rep.long_str.data;
        }
        constexpr const Char* get_long_pointer() const noexcept {
            BS_VERIFY(is_long(), "string must be in long state when accessing long string pointer");
            return rep.long_str.data;
        }
        constexpr Char* get_short_pointer() noexcept {
            BS_VERIFY(!is_long(), "string must be in short state when accessing short string pointer");
            return rep.short_str.data;
        }
        constexpr const Char* get_short_pointer() const noexcept {
            BS_VERIFY(!is_long(), "string must be in short state when accessing short string pointer");
            return rep.short_str.data;
        }

        constexpr Char* get_pointer() noexcept {
            return is_long() ? get_long_pointer() : get_short_pointer();
        }
        constexpr const Char* get_pointer() const noexcept {
            return is_long() ? get_long_pointer() : get_short_pointer();
        }
    };
}

template<class Traits>
class stringt {
public:
    using value_type = typename Traits::char_type;
    using size_type = typename Traits::size_type;


    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = value_type*;
    using const_iterator = const value_type*;

    using traits_type = Traits;
private:
    static constexpr std::size_t container_alignment = traits_type::string_container_alignment;
public:
    using allocator_type = bs::aligned_allocator<value_type, container_alignment>;
private:
    // alignas(container_alignment) for small string optimization
    alignas(container_alignment) detail::string_representation<value_type, size_type, 0> rep;

    using self_string_view = bs::string_viewt<traits_type>;
    using optional_char_reference = std::optional<detail::reference_wrapper<value_type>>;
    using optional_char_const_reference = std::optional<detail::reference_wrapper<const value_type>>;
public:

    constexpr stringt() noexcept
        : rep{} {
        rep.set_short_state();
        rep.set_short_size(0);
    }

    explicit constexpr stringt(const const_pointer str, const size_type str_len) {
        init_with_size(str_len);
        traits_type::copy(data(), str, str_len);
    }

    constexpr stringt(const stringt& other) {
        init_with_size(other.size());
        traits_type::copy(data(), other.data(), other.size());
    }
    constexpr stringt(stringt&& other) noexcept {
        rep = other.rep;
        other.rep = {};
    }

    template<class Begin, class End, std::enable_if_t<detail::is_input_iterator<Begin> && !std::is_convertible_v<End, size_type>, int> = 0>
    explicit constexpr stringt(Begin first, End last) {
        if constexpr (detail::is_random_access_iterator<Begin>) {
            const size_type size = static_cast<size_type>(last - first);
            const const_pointer ptr = detail::to_address(first);
            init_with_size(size);
            traits_type::copy(data(), ptr, size);
        } else if constexpr (detail::is_input_iterator<Begin>) {
            rep.set_short_state();
            rep.set_short_size(0);

            std::size_t count = 0;
            do {
                if (!(first != last)) {
                    rep.set_short_size(count);
                    return;
                }
                traits_type::assign(rep.get_short_pointer()[count], *first);

                ++first;
                ++count;
            } while (count < rep.get_short_capacity());

            if (!(first != last)) {
                rep.set_short_size(count);
                return;
            }

            size_type alloc_cap = calculate_capacity(count);
            pointer alloc_data = allocate(alloc_cap);
            traits_type::copy(alloc_data, rep.get_short_pointer(), count);
            traits_type::assign(alloc_data[count], *first);
            ++first;
            ++count;

            while (first != last) {
                traits_type::assign(alloc_data[count], *first);

                ++count;
                if (count >= alloc_cap) {
                    const size_type new_cap = calculate_capacity(count);
                    const pointer new_data = allocate(new_cap);
                    traits_type::copy(new_data, alloc_data, alloc_cap);
                    deallocate(alloc_data, alloc_cap);
                    alloc_data = new_data;
                    alloc_cap = new_cap;
                }
                ++first;
            }
            rep.set_long_state();
            rep.set_long_capacity(alloc_cap);
            rep.set_long_pointer(alloc_data);
            rep.set_long_size(count);
        }
    }

    BS_FORCEINLINE
    explicit constexpr stringt(const self_string_view str_view) {
        init_with_size(str_view.size());
        traits_type::copy(data(), str_view.data(), str_view.size());
    }

    [[nodiscard]] static constexpr stringt filled(const value_type ch, const size_type count) {
        stringt out;
        out.init_with_size(count);
        traits_type::assign(out.data(), count, ch);
        return out;
    }
    [[nodiscard]] static constexpr stringt with_capacity(const size_type cap) {
        stringt out;
        out.init_with_capacity(cap);
        return out;
    }
    [[nodiscard]] static constexpr stringt transfer_ownership(const pointer ptr, const size_type cap) noexcept {
        stringt out;
        out.rep.set_long_state();
        out.rep.set_long_pointer(ptr);
        out.rep.set_long_capacity(cap);
        out.rep.set_long_size(0);
        return out;
    }
    [[nodiscard]] static constexpr stringt transfer_ownership(const pointer ptr, const size_type size, const size_type cap) noexcept {
        stringt out;
        out.rep.set_long_state();
        out.rep.set_long_pointer(ptr);
        out.rep.set_long_capacity(cap);
        out.rep.set_long_size(size);
        return out;
    }
    [[nodiscard]] static constexpr stringt from_c_string(const const_pointer c_str) {
        BS_VERIFY(c_str != nullptr, "c_str is null pointer");
        return stringt{c_str, traits_type::length(c_str)};
    }
    static constexpr stringt from_c_string(std::nullptr_t) = delete;

    BS_CONSTEXPR_CXX20 ~stringt() noexcept {
        if (rep.is_long()) {
            deallocate(rep.get_long_pointer(), rep.get_long_capacity());
        }
    }

    constexpr stringt& operator=(const self_string_view str_view) {
        copy_from_buffer(str_view.data(), str_view.size());
        return *this;
    }

    constexpr stringt& operator=(const stringt& str) {
        if (this == &str) { return *this; }
        copy_from_independent(str.data(), str.size());
        return *this;
    }

    constexpr stringt& operator=(stringt&& str) noexcept {
        if (this == &str) { return *this; }
        if (rep.is_long()) { deallocate(rep.get_long_pointer(), rep.get_long_capacity()); }
        rep = str.rep;
        str.rep = {};
        return *this;
    }

    constexpr void reserve(const size_type new_req_cap) {
        const size_type old_cap = capacity();
        if (new_req_cap <= old_cap) { return; }

        const size_type new_cap = calculate_capacity(new_req_cap);
        const pointer new_data = allocate(new_cap);

        if (!rep.is_long()) {
            const size_type short_size = rep.get_short_size();
            traits_type::copy(new_data, rep.get_short_pointer(), short_size);
            rep.set_long_state();
            rep.set_long_pointer(new_data);
            rep.set_long_capacity(new_cap);
            rep.set_long_size(short_size);
            return;
        }
        traits_type::copy(new_data, rep.get_long_pointer(), rep.get_long_size());
        deallocate(rep.get_long_pointer(), old_cap);
        rep.set_long_pointer(new_data);
        rep.set_long_capacity(new_cap);
    }

    constexpr void reserve_add(const size_type additional_cap) {
        reserve(size() + additional_cap);
    }

    constexpr void reserve_exact(const size_type new_cap) {
        const size_type old_cap = capacity();
        if (new_cap <= old_cap) { return; }

        const pointer new_data = allocate(new_cap);

        if (!rep.is_long()) {
            const size_type short_size = rep.get_short_size();
            traits_type::copy(new_data, rep.get_short_pointer(), short_size);
            rep.set_long_state();
            rep.set_long_pointer(new_data);
            rep.set_long_capacity(new_cap);
            rep.set_long_size(short_size);
            return;
        }
        traits_type::copy(new_data, rep.get_long_pointer(), rep.get_long_size());
        deallocate(rep.get_long_pointer(), old_cap);
        rep.set_long_pointer(new_data);
        rep.set_long_capacity(new_cap);
    }

    constexpr void reserve_exact_add(const size_type additional_cap) {
        reserve_exact(size() + additional_cap);
    }

    constexpr void clear() noexcept {
        rep.set_size(0);
    }

    constexpr void push_back(const value_type ch) {
        if (rep.is_long()) {
            if (rep.get_long_size() == rep.get_long_capacity()) {
                const size_type new_cap = calculate_capacity(rep.get_long_size() + 1);
                const pointer new_data = allocate(new_cap);
                traits_type::copy(new_data, rep.get_long_pointer(), rep.get_long_size());
                deallocate(rep.get_long_pointer(), rep.get_long_capacity());
                rep.set_long_pointer(new_data);
                rep.set_long_capacity(new_cap);
            }
            rep.get_long_pointer()[rep.get_long_size()] = ch;
            rep.set_long_size(rep.get_long_size() + 1);
        } else {
            const size_type short_size = rep.get_short_size();
            if (rep.get_short_size() == rep.get_short_capacity()) {
                const size_type new_cap = calculate_capacity(rep.get_short_capacity() + 1);
                const pointer new_data = allocate(new_cap);
                traits_type::copy(new_data, rep.get_short_pointer(), rep.get_short_size());
                rep.set_long_state();
                rep.set_long_pointer(new_data);
                rep.set_long_capacity(new_cap);
                traits_type::assign(rep.get_long_pointer()[short_size], ch);
                rep.set_long_size(short_size + 1);
            } else {
                traits_type::assign(rep.get_short_pointer()[short_size], ch);
                rep.set_short_size(short_size + 1);
            }
        }
    }

    constexpr void pop_back() noexcept {
        BS_VERIFY(size() >= 1, "removing character from already empty string");
        rep.set_size(rep.get_size() - 1);
    }

    constexpr void append(const const_pointer str, const size_type str_len) {
        const size_type old_size = size();
        const size_type new_size = old_size + str_len;
        if (new_size <= capacity()) {
            traits_type::move(data() + size(), str, str_len);
            rep.set_size(new_size);
            return;
        }
        if (rep.is_long()) {
            const size_type new_cap = calculate_capacity(new_size);
            const pointer new_data = allocate(new_cap);
            traits_type::copy(new_data, rep.get_long_pointer(), old_size);
            traits_type::copy(new_data + old_size, str, str_len);
            deallocate(rep.get_long_pointer(), rep.get_long_capacity());
            rep.set_long_pointer(new_data);
            rep.set_long_capacity(new_cap);
            rep.set_long_size(new_size);
        } else {
            const size_type new_cap = calculate_capacity(new_size);
            const pointer new_data = allocate(new_cap);
            traits_type::copy(new_data, rep.get_short_pointer(), old_size);
            traits_type::copy(new_data + old_size, str, str_len);
            rep.set_long_state();
            rep.set_long_pointer(new_data);
            rep.set_long_capacity(new_cap);
            rep.set_long_size(new_size);
        }
    }
    constexpr void append(const self_string_view str_view) {
        this->append(str_view.data(), str_view.size());
    }
    template<class Begin, class End, std::enable_if_t<detail::is_random_access_iterator<Begin> && !std::is_convertible_v<End, size_type>, int> = 0>
    constexpr void append(Begin first, End last) {
        this->append(detail::to_address(first), static_cast<size_type>(last - first));
    }

    constexpr self_string_view substr(const size_type position) const noexcept BS_LIFETIMEBOUND {
        BS_VERIFY(position <= size(), "the start position of the substring exceeds the length of the string");
        return self_string_view{data() + position, size() - position};
    }
    constexpr self_string_view substr(const size_type position, const size_type count) const noexcept BS_LIFETIMEBOUND {
        BS_VERIFY(position <= size(), "the start position of the substring exceeds the length of the string");
        BS_VERIFY(count - position <= size(), "the length of substring exceeds the length of the string");
        return self_string_view{data() + position, count};
    }

    constexpr bool contains(const value_type ch) const noexcept {
        return traits_type::find(data(), size(), ch) != nullptr;
    }
    constexpr bool contains(const self_string_view str) const noexcept {
        return traits_type::findstr(data(), size(), str.data(), str.size()) != nullptr;
    }

    constexpr bool starts_with(const value_type ch) const noexcept {
        if (size() == 0) { return false; }
        return traits_type::eq(data()[0], ch);
    }
    constexpr bool starts_with(const self_string_view str) const noexcept {
        if (size() < str.size()) { return false; }
        return traits_type::compare(data(), str.data(), str.size()) == 0;
    }

    constexpr bool ends_with(const value_type ch) const noexcept {
        if (size() == 0) { return false; }
        return traits_type::eq(data()[size() - 1], ch);
    }
    constexpr bool ends_with(const self_string_view str) const noexcept {
        if (size() < str.size()) { return false; }
        return traits_type::compare(data() + (size() - str.size()), str.data(), str.size()) == 0;
    }

    constexpr pointer data() noexcept BS_LIFETIMEBOUND {
        return rep.get_pointer();
    }
    constexpr const_pointer data() const noexcept BS_LIFETIMEBOUND {
        return rep.get_pointer();
    }

    constexpr size_type size() const noexcept {
        return rep.get_size();
    }
    constexpr size_type capacity() const noexcept {
        return rep.get_capacity();
    }

    constexpr iterator begin() noexcept BS_LIFETIMEBOUND { return data(); }
    constexpr const_iterator begin() const noexcept BS_LIFETIMEBOUND { return data(); }
    constexpr iterator end() noexcept BS_LIFETIMEBOUND { return data() + size(); }
    constexpr const_iterator end() const noexcept BS_LIFETIMEBOUND { return data() + size(); }

    template<class Int, std::enable_if_t<std::is_integral_v<Int>, int> = 0>
    constexpr const_reference operator[](const Int index) const noexcept BS_LIFETIMEBOUND {
        BS_VERIFY((index + Int(size())) >= 0 && index < Int(size()), "index is out of range");
        return data()[index < 0 ? index + Int(size()) : index];
    }
    template<class Int, std::enable_if_t<std::is_integral_v<Int>, int> = 0>
    constexpr reference operator[](const Int index) noexcept BS_LIFETIMEBOUND {
        BS_VERIFY((index + Int(size())) >= 0 && index < Int(size()), "index is out of range");
        return data()[index < 0 ? index + Int(size()) : index];
    }

    template<class Int, std::enable_if_t<std::is_integral_v<Int>, int> = 0>
    constexpr optional_char_const_reference at(const Int index) const noexcept BS_LIFETIMEBOUND {
        if (index + Int(size()) < 0 || index >= Int(size())) {
            return std::nullopt;
        }
        return data()[index < 0 ? index + Int(size()) : index];
    }
    template<class Int, std::enable_if_t<std::is_integral_v<Int>, int> = 0>
    constexpr optional_char_reference at(const Int index) noexcept BS_LIFETIMEBOUND {
        if (index + Int(size()) < 0 || index >= Int(size())) {
            return std::nullopt;
        }
        return data()[index < 0 ? index + Int(size()) : index];
    }

    constexpr const_reference front() const noexcept BS_LIFETIMEBOUND {
        BS_VERIFY(size() >= 1, "cannot access the first element from an empty string");
        return data()[0];
    }
    constexpr reference front() noexcept BS_LIFETIMEBOUND {
        BS_VERIFY(size() >= 1, "cannot access the first element from an empty string");
        return data()[0];
    }
    constexpr const_reference back() const noexcept BS_LIFETIMEBOUND {
        BS_VERIFY(size() >= 1, "cannot access the last element from an empty string");
        return data()[size() - 1];
    }
    constexpr reference back() noexcept BS_LIFETIMEBOUND {
        BS_VERIFY(size() >= 1, "cannot access the last element from an empty string");
        return data()[size() - 1];
    }

    constexpr optional_char_const_reference at_front() const noexcept BS_LIFETIMEBOUND {
        if (size() == 0) { return std::nullopt; }
        return data()[0];
    }
    constexpr optional_char_reference at_front() noexcept BS_LIFETIMEBOUND {
        if (size() == 0) { return std::nullopt; }
        return data()[0];
    }

    constexpr optional_char_const_reference at_back() const noexcept BS_LIFETIMEBOUND {
        if (size() == 0) { return std::nullopt; }
        return data()[size() - 1];
    }
    constexpr optional_char_reference at_back() noexcept BS_LIFETIMEBOUND {
        if (size() == 0) { return std::nullopt; }
        return data()[size() - 1];
    }

    constexpr operator bs::string_viewt<traits_type>() const noexcept BS_LIFETIMEBOUND {
        return bs::string_viewt<traits_type>{data(), size()};
    }

private:
    using alloc_traits = std::allocator_traits<allocator_type>;

    BS_FORCEINLINE BS_FLATTEN
    constexpr void init_with_size(const size_type count) noexcept {
        if (rep.fits_in_sso(count)) {
            rep.set_short_state();
            rep.set_short_size(count);
        } else {
            rep.set_long_state();
            const size_type cap = calculate_capacity(count);
            rep.set_long_pointer(allocate(cap));
            rep.set_long_capacity(cap);
            rep.set_long_size(count);
        }
    }
    BS_FORCEINLINE BS_FLATTEN
    constexpr void init_with_capacity(const size_type cap) noexcept {
        if (rep.fits_in_sso(cap)) {
            rep.set_short_state();
            rep.set_short_size(0);
        } else {
            rep.set_long_state();
            rep.set_long_pointer(allocate(cap));
            rep.set_long_capacity(cap);
            rep.set_long_size(0);
        }
    }
    BS_FORCEINLINE BS_FLATTEN
    constexpr void copy_from_buffer(const value_type* const buf, const size_type buf_len) {
        size_type cap = rep.get_capacity();
        if (cap >= buf_len) {
            traits_type::move(rep.get_pointer(), buf, buf_len);
            rep.set_size(buf_len);
        } else if (!rep.is_long()) {
            const size_type new_cap = calculate_capacity(buf_len);
            const pointer new_data = allocate(new_cap);
            traits_type::copy(new_data, buf, buf_len);
            rep.set_long_state();
            rep.set_long_pointer(new_data);
            rep.set_long_capacity(new_cap);
            rep.set_long_size(buf_len);
        } else {
            deallocate(rep.get_long_pointer(), cap);
            const size_type new_cap = calculate_capacity(cap);
            const pointer new_data = allocate(new_cap);
            traits_type::copy(new_data, buf, buf_len);
            rep.set_long_pointer(new_data);
            rep.set_long_capacity(new_cap);
            rep.set_long_size(buf_len);
        }
    }
    BS_FORCEINLINE BS_FLATTEN
    constexpr void copy_from_independent(const value_type* const buf, const size_type buf_len) {
        size_type cap = rep.get_capacity();
        if (cap >= buf_len) {
            traits_type::copy(rep.get_pointer(), buf, buf_len);
            rep.set_size(buf_len);
        } else if (!rep.is_long()) {
            const size_type new_cap = calculate_capacity(buf_len);
            const pointer new_data = allocate(new_cap);
            traits_type::copy(new_data, buf, buf_len);
            rep.set_long_state();
            rep.set_long_pointer(new_data);
            rep.set_long_capacity(new_cap);
            rep.set_long_size(buf_len);
        } else {
            deallocate(rep.get_long_pointer(), cap);
            const size_type new_cap = calculate_capacity(buf_len);
            const pointer new_data = allocate(new_cap);
            traits_type::copy(new_data, buf, buf_len);
            rep.set_long_pointer(new_data);
            rep.set_long_capacity(new_cap);
            rep.set_long_size(buf_len);
        }
    }

    static constexpr size_type calculate_capacity(const size_type req_cap) noexcept {
        BS_VERIFY(req_cap != size_type(-1), "exceeded maximum allowed capacity");
        const size_type new_cap = req_cap * 2;
        // if overflows
        if (new_cap < req_cap) { return size_type(-1); }
        return new_cap;
    }
    [[nodiscard]] constexpr pointer allocate(const size_type cap) noexcept {
        static_assert(alloc_traits::is_always_equal::value);

        allocator_type alloc{};
        return alloc_traits::allocate(alloc, cap);
    }
    constexpr void deallocate(const pointer ptr, const size_type cap) noexcept {
        static_assert(alloc_traits::is_always_equal::value);

        allocator_type alloc{};
        alloc_traits::deallocate(alloc, ptr, cap);
    }
};

template<class Tr>
constexpr bool operator==(const stringt<Tr>& left, const stringt<Tr>& right) noexcept {
    if (left.size() != right.size()) { return false; }
    return Tr::compare(left.data(), right.data(), left.size()) == 0;
}
template<class Tr, std::size_t N>
constexpr bool operator==(const stringt<Tr>& left, const typename Tr::char_type(&right)[N]) noexcept {
    static_assert(N != 0, "given non-null terminated array");
    if (left.size() != (N - 1)) { return false; }
    return Tr::compare(left.data(), right, (N - 1)) == 0;
}
template<class Tr, std::size_t N>
constexpr bool operator==(const typename Tr::char_type(&left)[N], const stringt<Tr>& right) noexcept {
    static_assert(N != 0, "given non-null terminated array");
    if (right.size() != (N - 1)) { return false; }
    return Tr::compare(left, right.size(), (N - 1)) == 0;
}

template<class Tr>
constexpr bool operator!=(const stringt<Tr>& left, const stringt<Tr>& right) noexcept {
    return !(left == right);
}
template<class Tr, std::size_t N>
constexpr bool operator!=(const stringt<Tr>& left, const typename Tr::char_type(&right)[N]) noexcept {
    return !(left == right);
}
template<class Tr, std::size_t N>
constexpr bool operator!=(const typename Tr::char_type(&left)[N], const stringt<Tr>& right) noexcept {
    return !(left == right);
}


using string = stringt<char_traits<char>>;



inline namespace literals {
    BS_CONSTEXPR_CXX20 bs::string operator ""_s(const char* const str, const std::size_t length) {
        return bs::string{str, length};
    }
}


}
