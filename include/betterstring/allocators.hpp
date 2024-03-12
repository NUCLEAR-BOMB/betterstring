#pragma once

#include <betterstring/detail/preprocessor.hpp>
#include <cstddef>
#include <new>
#include <type_traits>

namespace bs {

template<class T, std::size_t Alignment>
class aligned_allocator {
public:
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
        return static_cast<T*>(::operator new[](sizeof(T) * n, std::align_val_t{Alignment}));
    }
    constexpr void deallocate(T* const ptr, const std::size_t n) {
        ::operator delete[](ptr, sizeof(T) * n, std::align_val_t{Alignment});
    }
};

}
