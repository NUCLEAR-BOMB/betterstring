#pragma once

#include <betterstring/detail/preprocessor.hpp>
#include <stdexcept>
#include <cstddef>

namespace bs {

template<class CharT>
class find_result {
public:
    using size_type = std::size_t;
    using pointer = CharT*;

    constexpr find_result(const pointer string_data_, const size_type string_end_, const pointer find_ptr_) noexcept
        : string_data(string_data_), string_end(string_end_), find_ptr(find_ptr_) {}

    constexpr size_type index() const noexcept {
        BS_VERIFY(find_ptr != nullptr, "index() is called when the result is not found");
        return static_cast<size_type>(find_ptr - string_data);
    }
    constexpr size_type index_or_end() const noexcept {
        if (find_ptr == nullptr) { return string_end; }
        return static_cast<size_type>(find_ptr - string_data);
    }
    constexpr size_type index_or_npos() const noexcept {
        if (find_ptr == nullptr) { return static_cast<size_type>(-1); }
        return static_cast<size_type>(find_ptr - string_data);
    }
    constexpr size_type index_or_throw() const {
        if (find_ptr == nullptr) {
            throw std::logic_error("result is not found");
        }
        return static_cast<size_type>(find_ptr - string_data);
    }

    constexpr operator size_type() const noexcept {
        return index_or_end();
    }

    constexpr pointer ptr() const noexcept {
        BS_VERIFY(find_ptr != nullptr, "ptr() is called when the result is not found");
        return find_ptr;
    }
    constexpr pointer ptr_or_end() const noexcept {
        if (find_ptr == nullptr) { return string_data + string_end; }
        return find_ptr;
    }
    constexpr pointer ptr_or_null() const noexcept {
        return find_ptr;
    }
    constexpr pointer ptr_or_throw() const {
        if (find_ptr == nullptr) {
            throw std::logic_error("result is not found");
        }
        return find_ptr;
    }

    constexpr bool found() const noexcept {
        return find_ptr != nullptr;
    }

private:
    pointer string_data;
    size_type string_end;
    pointer find_ptr;
};

}
