#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <exception>

#include <betterstring/detail/parse_unsigned.hpp>

namespace bs {

struct bad_parse : std::exception {
    parse_error err;

    bad_parse(const parse_error err_) noexcept : err{err_} {}
};

template<class T>
class parse_result {
private:
    T val;
    parse_error err;

    template<class> friend class parse_result;
public:
    constexpr parse_result(const parse_error err) noexcept : val(0), err(err) {}
    constexpr parse_result(const T result) noexcept : val(result), err(parse_error{}) {}

    explicit constexpr parse_result(const T result, const parse_error err) noexcept
        : val(result), err(err) {}

    template<class U>
    explicit constexpr parse_result(const parse_result<U> other) noexcept
        : val(static_cast<T>(other.val)), err(other.err) {}

    constexpr T value() const noexcept {
        BS_VERIFY(err == parse_error{}, "");
        return val;
    }
    constexpr T unchecked_value() const noexcept {
        return val;
    }
    constexpr T value_or_throw() const {
        if (err != parse_error{}) { throw bad_parse{err}; }
        return val;
    }

    constexpr parse_error error() const noexcept {
        return err;
    }

    constexpr bool has_error() const noexcept { return err != parse_error{}; }
    explicit constexpr operator bool() const noexcept { return err == parse_error{}; }

    friend constexpr bool operator==(const parse_result<T> left, const parse_result<T> right) noexcept {
        if (left.err == right.err) return true;
        return left.val == right.val;
    }
    friend constexpr bool operator!=(const parse_result<T> left, const parse_result<T> right) noexcept {
        return !(left == right);
    }
};

template<class T, class Ch>
constexpr parse_result<T> parse(const Ch* const str, const std::size_t count) {
    if constexpr (std::is_unsigned_v<T>) {
        if (detail::is_constant_evaluated()) {
            T result = 0;
            const auto err = bs::detail::constexpr_parse_unsigned<T>(result, str, count);
            return bs::parse_result<T>{result, err};
        } else {
            T result;
            const auto err = bs::detail::parse_unsigned<T>(result, str, count);
            return bs::parse_result<T>{result, err};
        }
    } else {
        static_assert(sizeof(T) == 0, "unimplemented");
    }
}

}
