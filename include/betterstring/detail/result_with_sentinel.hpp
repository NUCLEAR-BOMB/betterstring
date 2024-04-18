#pragma once

#include <betterstring/detail/preprocessor.hpp>
#include <exception>

namespace bs {

template<class E>
struct bad_result : std::exception {
    E err;

    bad_result(const E err_) noexcept : err{err_} {}
};

namespace detail {

template<class T, class E, E sentinel>
class result_with_sentinel {
protected:
    T val;
    E err;
public:
    using value_type = T;
    using error_type = E;

    constexpr result_with_sentinel(const error_type err_) noexcept : val{}, err{err_} {}
    constexpr result_with_sentinel(const value_type val_) noexcept : val{val_}, err{sentinel} {}

    explicit constexpr result_with_sentinel(const value_type val_, const error_type err_) noexcept
        : val{val_}, err{err_} {}

    constexpr value_type value() const noexcept {
        BS_VERIFY(err == sentinel, "called value() with no value");
        return val;
    }
    constexpr value_type value_or(const value_type default_val) const noexcept {
        if (err != sentinel) { return default_val; }
        return val;
    }

    constexpr value_type unchecked_value() const noexcept {
        return val;
    }
    constexpr value_type value_or_throw() const noexcept {
        if (err != sentinel) { throw bad_result<error_type>{}; }
        return val;
    }

    constexpr error_type error() const noexcept {
        return err;
    }

    constexpr bool has_error() const noexcept { return err != sentinel; }
    explicit constexpr operator bool() const noexcept { return err == sentinel; }

    friend constexpr bool operator==(const result_with_sentinel<T, E, sentinel> left, const result_with_sentinel<T, E, sentinel> right) noexcept {
        if (left.err == right.err) { return true; }
        return left.val == right.val;
    }
    friend constexpr bool operator!=(const result_with_sentinel<T, E, sentinel> left, const result_with_sentinel<T, E, sentinel> right) noexcept {
        return !(left == right);
    }
};

}

}
