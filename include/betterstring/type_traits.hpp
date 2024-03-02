#pragma once

#include <betterstring/detail/preprocessor.hpp>
#include <cstdint>
#include <type_traits>
#include <cstddef>

namespace bs {

namespace detail {
    constexpr bool is_constant_evaluated() noexcept {
#ifdef __cpp_lib_is_constant_evaluated
        return std::is_constant_evaluated();
#elif BS_COMP_CLANG || BS_COMP_GCC || BS_COMP_MSVC
        return __builtin_is_constant_evaluated();
#else
        return false;
#endif
    }

    template<class>
    inline constexpr bool always_false = false;

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
    using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

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

}
