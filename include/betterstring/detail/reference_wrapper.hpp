#pragma once

#include <type_traits>
#include <utility>
#include <memory>
#include <betterstring/type_traits.hpp>
#include <betterstring/detail/preprocessor.hpp>

namespace bs::detail {

template<class T>
class reference_wrapper {
public:
    using type = T;

private:
    T* ptr;

    static void fun(T&) noexcept;
    static void fun(T&&) = delete;
public:
    template<class U,
        class = std::void_t<decltype(fun(std::declval<U>()))>,
        std::enable_if_t<!std::is_same_v<remove_cvref_t<U>, reference_wrapper>, int> = 0
    >
    constexpr reference_wrapper(U&& u) noexcept(noexcept(fun(std::declval<U>()))) {
        T& f = static_cast<U&&>(u);
        ptr = std::addressof(f);
    }

    reference_wrapper(const reference_wrapper&) noexcept = default;

    reference_wrapper& operator=(const reference_wrapper& x) noexcept = default;

    // extension
    template<class U, std::enable_if_t<std::is_assignable_v<T&, U>, int> = 0>
    constexpr reference_wrapper& operator=(U&& other) noexcept(std::is_nothrow_assignable_v<T&, U>) {
        get() = static_cast<U&&>(other);
        return *this;
    }

    constexpr operator T&() const noexcept {
        BS_ASSUME(ptr != nullptr);
        return *ptr;
    }
    constexpr T& get() const noexcept {
        BS_ASSUME(ptr != nullptr);
        return *ptr;
    }

    template<class... Args>
    constexpr std::invoke_result_t<T&, Args...> operator()(Args&&... args) const
        noexcept(std::is_nothrow_invocable_v<T, Args...>) {
        BS_ASSUME(ptr != nullptr);
        return std::invoke(*ptr, std::forward<Args>(args)...);
    }
};

template<class T>
reference_wrapper(T&) -> reference_wrapper<T>;

}
