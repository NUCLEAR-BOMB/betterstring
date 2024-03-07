#pragma once

#include <betterstring/detail/preprocessor.hpp>
#include <stdexcept>
#include <cstddef>

namespace bs {

template<class CharT>
class find_result {
public:
    using size_type = std::size_t;

    constexpr find_result(const CharT* string_data_, const size_type string_size_, const CharT* find_ptr_) noexcept
        : string_data(string_data_), string_size(string_size_), find_ptr(find_ptr_) {}

    constexpr size_type index() const noexcept {
        BS_VERIFY(find_ptr != nullptr, "index() is called when the result is not found");
        return static_cast<size_type>(find_ptr - string_data);
    }
    constexpr size_type index_or_end() const noexcept {
        if (find_ptr == nullptr) return string_size;
        return static_cast<size_type>(find_ptr - string_data);
    }
    constexpr size_type index_or_npos() const noexcept {
        if (find_ptr == nullptr) return static_cast<size_type>(-1);
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

    constexpr const CharT* ptr() const noexcept {
        BS_VERIFY(find_ptr != nullptr, "ptr() is called when the result is not found");
        return find_ptr;
    }
    constexpr const CharT* ptr_or_end() const noexcept {
        if (find_ptr == nullptr) return string_data + string_size;
        return find_ptr;
    }
    constexpr const CharT* ptr_or_null() const noexcept {
        return find_ptr;
    }
    constexpr const CharT* ptr_or_throw() const {
        if (find_ptr == nullptr) {
            throw std::logic_error("result is not found");
        }
        return find_ptr;
    }

    constexpr bool found() const noexcept {
        return find_ptr != nullptr;
    }

private:
    const CharT* string_data;
    size_type string_size;
    const CharT* find_ptr;
};

}
