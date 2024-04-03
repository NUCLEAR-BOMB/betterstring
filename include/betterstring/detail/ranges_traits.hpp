#pragma once

#include <utility>
#include <type_traits>
#include <iterator>
#include <betterstring/detail/preprocessor.hpp>
#include <betterstring/type_traits.hpp>

namespace bs::detail::ranges {

template<class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template<class T>
using iterator_category = typename std::iterator_traits<T>::iterator_category;

template<class T, class = void>
inline constexpr bool is_input_or_output_iterator = false;
template<class T>
inline constexpr bool is_input_or_output_iterator<T, std::void_t<iterator_category<T>>> =
    std::is_base_of_v<std::input_iterator_tag, iterator_category<T>>
    || std::is_base_of_v<std::output_iterator_tag, iterator_category<T>>;

template<class T>
inline constexpr bool is_class_or_enum = std::is_class_v<T> || std::is_union_v<T> || std::is_enum_v<T>;

template<class T>
inline constexpr bool is_integer_like = std::is_integral_v<T> && !std::is_same_v<T, bool>;


template<class T, class = void>
inline constexpr bool is_default_initializable = false;
template<class T>
inline constexpr bool is_default_initializable<T, std::void_t<decltype(T{})>>
    = std::is_nothrow_destructible_v<T> && std::is_default_constructible_v<T>;

template<class T>
inline constexpr bool is_movable =
       std::is_object_v<T>
    && std::is_move_constructible_v<T>
    && std::is_nothrow_destructible_v<T>
    && std::is_convertible_v<T, T>;

template<class T>
inline constexpr bool is_copyable =
       std::is_copy_constructible_v<T>
    && is_movable<T>
    && std::is_assignable_v<T&, T&>
    && std::is_assignable_v<T&, const T&>
    && std::is_assignable_v<T&, const T>;

template<class T>
inline constexpr bool is_semiregular = is_copyable<T> && is_default_initializable<T>;

// template<class T1, class T2, class = void,
//     class T1Ref = const std::remove_reference_t<T1>&,
//     class T2Ref = const std::remove_reference_t<T2>&
// >
// inline constexpr bool is_weakly_equality_comparable_with = false;
// template<class T1, class T2, class T1Ref, class T2Ref>
// inline constexpr bool is_weakly_equality_comparable_with<T1, T2, std::void_t<decltype(
//     std::declval<T1Ref>() == std::declval<T2Ref>(),
//     std::declval<T1Ref>() != std::declval<T2Ref>(),
//     std::declval<T2Ref>() == std::declval<T1Ref>(),
//     std::declval<T2Ref>() != std::declval<T1Ref>()
//     )
// >, T1Ref, T1Ref> = std::is_convertible_v<decltype(std::declval<T1Ref>() == std::declval<T2Ref>()), bool>;

template<class S, class I>
inline constexpr bool is_sentinel_for = is_semiregular<S> && is_input_or_output_iterator<I>;

template<class T, class = void>
inline constexpr bool is_forward_iterator = false;
template<class T>
inline constexpr bool is_forward_iterator<T, std::void_t<iterator_category<T>>> =
    std::is_base_of_v<std::forward_iterator_tag, iterator_category<T>>;


template<class T, class = void>
inline constexpr bool is_random_access_iterator = false;
template<class T>
inline constexpr bool is_random_access_iterator<T, std::void_t<iterator_category<T>>> =
    std::is_base_of_v<std::random_access_iterator_tag, iterator_category<T>>;

template<class T>
constexpr std::make_unsigned_t<T> to_unsigned_like(T x) noexcept {
    return static_cast<std::make_unsigned_t<T>>(x);
}

namespace begin_impl {
#if BS_COMP_CLANG
    void begin() = delete;
#else
    void begin();
#endif
    template<class T, class = void>
    inline constexpr bool has_member_begin = false;
    template<class T>
    inline constexpr bool has_member_begin<T, std::void_t<decltype(std::declval<T>().begin())>>
        = is_input_or_output_iterator<decltype(std::declval<T>().begin())>;

    template<class T, class = void>
    inline constexpr bool has_adl_begin = false;
    template<class T>
    inline constexpr bool has_adl_begin<T, std::void_t<decltype(begin(std::declval<T>()))>> =
        !has_member_begin<T>
        && is_class_or_enum<remove_cvref_t<T>>
        && is_input_or_output_iterator<decltype(begin(std::declval<T>()))>;

    template<class T, std::enable_if_t<sizeof(T) >= 0, int> = 0>
    constexpr auto begin(T (&arr)[]) {
        return arr + 0;
    }
    template<class T, std::size_t N, std::enable_if_t<sizeof(T) >= 0, int> = 0>
    constexpr auto begin(T (&arr)[N]) {
        return arr + 0;
    }
    template<class T, std::enable_if_t<has_member_begin<T>, int> = 0>
    constexpr auto begin(T&& x) noexcept(noexcept(x.begin())) {
        return x.begin();
    }
    template<class T, std::enable_if_t<has_adl_begin<T>, int> = 0>
    constexpr auto begin(T&& x) noexcept(noexcept(begin(x))) {
        return begin(x);
    }
}

template<class T>
using iterator_t = decltype(begin_impl::begin(std::declval<T&>()));

using begin_impl::begin;

namespace end_impl {
#if BS_COMP_CLANG
    void end() = delete;
#else
    void end();
#endif

    template<class T, class = void>
    inline constexpr bool has_member_end = false;
    template<class T>
    inline constexpr bool has_member_end<T, std::void_t<decltype(
        std::declval<T>().end(),
        ranges::begin(std::declval<T&>())
    )
    >> = is_sentinel_for<
        decltype(std::declval<T>().end()),
        decltype(ranges::begin(std::declval<T&>()))
        >;

    template<class T, class = void>
    inline constexpr bool has_adl_end = false;
    template<class T>
    inline constexpr bool has_adl_end<T, std::void_t<decltype(
        end(std::declval<T>()),
        ranges::begin(std::declval<T&>())
    )
    >> = !has_member_end<T>
        && is_class_or_enum<remove_cvref_t<T>>
        && is_sentinel_for<
            decltype(end(std::declval<T>())),
            decltype(ranges::begin(std::declval<T&>()))
        >;

    template<class T, std::size_t N, std::enable_if_t<sizeof(T) >= 0, int> = 0>
    constexpr auto end(T (&arr)[N]) {
        return arr + N;
    }
    template<class T, std::enable_if_t<has_member_end<T>, int> = 0>
    constexpr auto end(T&& x) noexcept(noexcept(x.end())) {
        return x.end();
    }
    template<class T, std::enable_if_t<has_adl_end<T>, int> = 0>
    constexpr auto end(T&& x) noexcept(noexcept(end(x))) {
        return end(x);
    }
}

using end_impl::end;

namespace size_impl {
#if BS_COMP_CLANG
    void size() = delete;
#else
    void size();
#endif

    template<class T, class = void>
    inline constexpr bool has_member_size = false;
    template<class T>
    inline constexpr bool has_member_size<T, std::void_t<decltype(std::declval<T>().size())>>
        = is_integer_like<decltype(std::declval<T>().size())>;

    template<class T, class = void>
    inline constexpr bool has_adl_size = false;
    template<class T>
    inline constexpr bool has_adl_size<T, std::void_t<decltype(size(std::declval<T>()))>> =
        !has_member_size<T>
        && is_class_or_enum<remove_cvref_t<T>>
        && is_integer_like<decltype(size(std::declval<T>()))>;

    template<class T, class = void>
    inline constexpr bool has_difference = false;
    template<class T>
    inline constexpr bool has_difference<T, std::void_t<decltype(
        ranges::begin(std::declval<T>()),
        ranges::end(std::declval<T>())
    )>> = !has_adl_size<T> && !has_member_size<T>
        && is_class_or_enum<remove_cvref_t<T>>
        && is_forward_iterator<decltype(ranges::begin(std::declval<T>()))>
        && is_sentinel_for<
            decltype(ranges::end(std::declval<T>())),
            decltype(ranges::begin(std::declval<T>()))
        >;

    template<class T, std::size_t N>
    constexpr auto size(T (&)[N]) noexcept {
        return N;
    }
    template<class T, std::size_t N>
    constexpr auto size(T (&&)[N]) noexcept {
        return N;
    }
    template<class T, std::enable_if_t<has_member_size<T>, int> = 0>
    constexpr auto size(T&& x) noexcept(noexcept(x.size())) {
        return x.size();
    }
    template<class T, std::enable_if_t<has_adl_size<T>, int> = 0>
    constexpr auto size(T&& x) noexcept(noexcept(size(x))) {
        return size(x);
    }
    template<class T, std::enable_if_t<has_difference<T>, int> = 0>
    constexpr auto size(T&& x) noexcept(noexcept(to_unsigned_like(ranges::end(x) - ranges::begin(x))))
        -> decltype(to_unsigned_like(ranges::end(x) - ranges::begin(x))) {
        return to_unsigned_like(ranges::end(x) - ranges::begin(x));
    }
}

using size_impl::size;

namespace data_impl {
    template<class T>
    inline constexpr bool is_pointer_and_object = std::is_pointer_v<T> && std::is_object_v<std::remove_pointer_t<T>>;

    template<class T, class = void>
    inline constexpr bool has_member_data = false;
    template<class T>
    inline constexpr bool has_member_data<T, std::void_t<decltype(std::declval<T>().data())>>
        = is_pointer_and_object<decltype(std::declval<T>().data())>;

    template<class T, class = void>
    inline constexpr bool is_begin_invocable = false;
    template<class T>
    inline constexpr bool is_begin_invocable<T, std::void_t<decltype(ranges::begin(std::declval<T>()))>>
        = !has_member_data<T> && is_random_access_iterator<decltype(ranges::begin(std::declval<T>()))>;

    template<class T, std::enable_if_t<has_member_data<T>, int> = 0>
    constexpr auto data(T&& x) noexcept(noexcept(x.data())) {
        return x.data();
    }
    template<class T, std::enable_if_t<is_begin_invocable<T>, int> = 0>
    constexpr auto data(T&& x) noexcept(noexcept(detail::to_address(ranges::begin(x))))
        -> decltype(detail::to_address(ranges::begin(x))) {
        return detail::to_address(ranges::begin(x));
    }
}
using data_impl::data;

template<class R, class = void>
inline constexpr bool is_random_access_range = false;
template<class R>
inline constexpr bool is_random_access_range<R, std::void_t<
    decltype(detail::ranges::begin(std::declval<R&>()))
>> = is_random_access_iterator<decltype(detail::ranges::begin(std::declval<R&>()))>;

template<class R>
inline constexpr bool is_contiguous_range = is_random_access_range<R>;

}
