#pragma once

#include <climits>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <optional>

#include <betterstring/char_traits.hpp>
#include <betterstring/type_traits.hpp>
#include <betterstring/string_view.hpp>
#include <betterstring/detail/preprocessor.hpp>

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
            return size < short_capacity;
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

    template<class Tr, class string_view_like>
    inline constexpr bool is_string_view_convertible =
        std::is_convertible_v<const string_view_like&, bs::string_view<Tr>>
        && !std::is_convertible_v<const string_view_like&, const typename Tr::char_type*>;
}

template<class Traits>
class stringt {
public:
    using value_type = typename Traits::char_type;
    using size_type = typename Traits::size_type;

    using allocator_type = std::allocator<value_type>;

    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = value_type*;
    using const_iterator = const value_type*;

    using traits_type = Traits;

private:
    detail::string_representation<value_type, size_type, 0> rep;

    using self_string_view = bs::string_view<traits_type>;
    using optional_char = std::optional<value_type>;
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

    template<class Begin, class End, std::enable_if_t<!std::is_convertible_v<End, size_type>, int> = 0>
    explicit constexpr stringt(Begin first, End last) {
        const size_type first_size = static_cast<size_type>(last - first);
        const const_pointer first_ptr = detail::to_address(first);
        init_with_size(first_size);
        traits_type::copy(data(), first_ptr, first_size);
    }

    template<class string_view_like, std::enable_if_t<
        detail::is_string_view_convertible<traits_type, string_view_like>
    , int> = 0>
    BS_FORCEINLINE
    explicit constexpr stringt(const string_view_like& t) {
        const self_string_view str_view = t;

        init_with_size(str_view.size());
        traits_type::copy(data(), str_view.data(), str_view.size());
    }

    [[nodiscard]] static constexpr stringt filled(const value_type ch, const size_type count) noexcept {
        stringt out;
        out.init_with_size(count);
        traits_type::assign(out.data(), count, ch);
        return out;
    }
    [[nodiscard]] static constexpr stringt with_capacity(const size_type cap) noexcept {
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

    ~stringt() noexcept {
        if (rep.is_long()) {
            deallocate(rep.get_long_pointer(), rep.get_long_capacity());
        }
    }

    template<class string_view_like, std::enable_if_t<
        detail::is_string_view_convertible<traits_type, string_view_like>
    , int> = 0>
    constexpr stringt& operator=(const string_view_like& t) {
        const self_string_view str_view = t;
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
                rep.get_long_pointer()[short_size] = ch;
                rep.set_long_size(short_size + 1);
            } else {
                rep.get_short_pointer()[short_size] = ch;
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
    template<class string_view_like, std::enable_if_t<
        detail::is_string_view_convertible<traits_type, string_view_like>
        && !std::is_convertible_v<string_view_like, const_pointer>
    , int> = 0>
    constexpr void append(const string_view_like& t) {
        const self_string_view str_view = t;
        append(str_view.data(), str_view.size());
    }
    constexpr void append(const const_pointer nt_str) {
        append(nt_str, detail::strlen_elision(nt_str));
    }
    template<class Begin, class End, std::enable_if_t<!std::is_convertible_v<End, size_type>, int> = 0>
    constexpr void append(Begin first, End last) {
        append(detail::to_address(first), static_cast<size_type>(last - first));
    }

    constexpr pointer data() noexcept {
        return rep.get_pointer();
    }
    constexpr const_pointer data() const noexcept {
        return rep.get_pointer();
    }

    constexpr size_type size() const noexcept {
        return rep.get_size();
    }
    constexpr size_type capacity() const noexcept {
        return rep.get_capacity();
    }

    constexpr iterator begin() noexcept { return data(); }
    constexpr const_iterator begin() const noexcept { return data(); }
    constexpr iterator end() noexcept { return data() + size(); }
    constexpr const_iterator end() const noexcept { return data() + size(); }

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

    template<class Int, std::enable_if_t<std::is_integral_v<Int>, int> = 0>
    constexpr optional_char at(const Int index) const noexcept {
        if (index + Int(size()) < 0 || index >= Int(size())) {
            return {};
        }
        return data()[index < 0 ? index + size() : index];
    }

    constexpr const_reference front() const noexcept {
        BS_VERIFY(size() >= 1, "cannot access the first element from an empty string");
        return data()[0];
    }
    constexpr reference front() noexcept {
        BS_VERIFY(size() >= 1, "cannot access the first element from an empty string");
        return data()[0];
    }
    constexpr const_reference back() const noexcept {
        BS_VERIFY(size() >= 1, "cannot access the last element from an empty string");
        return data()[size() - 1];
    }
    constexpr reference back() noexcept {
        BS_VERIFY(size() >= 1, "cannot access the last element from an empty string");
        return data()[size() - 1];
    }

    constexpr optional_char at_front() const noexcept {
        if (size() == 0) { return {}; }
        return data()[0];
    }
    constexpr optional_char at_back() const noexcept {
        if (size() == 0) { return {}; }
        return data()[size() - 1];
    }

    constexpr allocator_type get_allocator() const noexcept {
        return rep.get_allocator();
    }

    constexpr operator bs::string_view<traits_type>() const noexcept {
        return bs::string_view<traits_type>{data(), size()};
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
        allocator_type alloc{};
        return alloc_traits::allocate(alloc, cap);
    }
    constexpr void deallocate(const pointer ptr, const size_type cap) noexcept {
        allocator_type alloc{};
        alloc_traits::deallocate(alloc, ptr, cap);
    }
};

template<class Tr>
constexpr bool operator==(const stringt<Tr>& left, const stringt<Tr>& right) noexcept {
    if (left.size() != right.size()) { return false; }
    return Tr::compare(left.data(), right.data(), left.size()) == 0;
}

template<class Tr>
constexpr bool operator!=(const stringt<Tr>& left, const stringt<Tr>& right) noexcept {
    return !(left == right);
}

using string = stringt<char_traits<char>>;



inline namespace literals {
    BS_CONSTEXPR_CXX20 bs::string operator ""_s(const char* const str, const std::size_t length) {
        return bs::string{str, length};
    }
}


}
