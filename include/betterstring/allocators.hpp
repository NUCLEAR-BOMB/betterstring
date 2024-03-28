
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <betterstring/detail/preprocessor.hpp>
#include <cstddef>
#include <new>
#include <type_traits>

namespace bs {

namespace detail {
    template<class T>
    constexpr bool is_power_of_two(const T x) noexcept {
        return x != 0 && (x & (x - 1)) == 0;
    }
}

template<class T, std::size_t Alignment>
class aligned_allocator {
public:
    static_assert(detail::is_power_of_two(Alignment), "The alignment must be representable to the power of two");

    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;

    template<class U>
    struct rebind { using other = aligned_allocator<U, Alignment>; };

    constexpr aligned_allocator() noexcept = default;
    constexpr aligned_allocator(const aligned_allocator&) noexcept = default;
    template<class U>
    constexpr aligned_allocator(const aligned_allocator<U, Alignment>&) noexcept {}

    constexpr T* allocate(const std::size_t n) {
        if (std::size_t(-1) / sizeof(T) < n) { throw std::bad_array_new_length{}; }

        if constexpr (Alignment == 1) {
#ifdef BS_BUILTIN_OPERATOR_NEW
            return static_cast<T*>(BS_BUILTIN_OPERATOR_NEW(sizeof(T) * n));
#else
            return static_cast<T*>(::operator new[](sizeof(T) * n));
#endif
        } else {
#ifdef BS_BUILTIN_OPERATOR_NEW
            return static_cast<T*>(BS_BUILTIN_OPERATOR_NEW(sizeof(T) * n, std::align_val_t{Alignment}));
#else
            return static_cast<T*>(::operator new[](sizeof(T) * n, std::align_val_t{Alignment}));
#endif
        }
    }
    constexpr void deallocate(T* const ptr, [[maybe_unused]] const std::size_t n) {
        if constexpr (Alignment == 1) {
#ifdef BS_BUILTIN_OPERATOR_DELETE
            BS_BUILTIN_OPERATOR_DELETE(ptr);
#else
            ::operator delete[](ptr, sizeof(T) * n);
#endif
        } else {
#ifdef BS_BUILTIN_OPERATOR_DELETE
            BS_BUILTIN_OPERATOR_DELETE(ptr, std::align_val_t{Alignment});
#else
            ::operator delete[](ptr, sizeof(T) * n, std::align_val_t{Alignment});
#endif
        }
    }
};

}
