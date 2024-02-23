#include <cinttypes>
#include <cassert>
#include <new>

#include <betterstring/functions.hpp>

uint16_t bit_ceil_u16(uint16_t num) {
    --num;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    ++num;
    return num;
}

template<class T>
T* aligned_new(const size_t num, const size_t alignment) {
    return static_cast<T*>(::operator new[](sizeof(T)* num, std::align_val_t{alignment}));
}
template<class T>
void aligned_delete(T* const ptr, const size_t num, const size_t alignment) {
    ::operator delete[](ptr, num, std::align_val_t{alignment});
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
    if (Size < 2) { return -1; }

    const char search_for = reinterpret_cast<const char&>(Data[0]);
    if (reinterpret_cast<const uint8_t&>(Data[1]) == 0) { return -1; }
    // const uint16_t alignment = bit_ceil_u16(reinterpret_cast<const uint8_t&>(Data[1]));
    const uint16_t alignment = 1;
    
    const size_t str_len = Size - 2;
    char* const str = aligned_new<char>(str_len, alignment);
    std::memcpy(str, Data + 2, str_len);

    const char* result = bs::strfindn(str, str_len, search_for);

    if (result == nullptr) {
        for (size_t i = 0; i < str_len; ++i) {
            if (str[i] != search_for) {
                std::abort();
            }
        }
    } else {
        if (*result == search_for) {
            std::abort();
        }

        for (auto it = str; it != result; ++it) {
            if (*it != search_for) {
                std::abort();
            }
        }
    }

    aligned_delete(str, str_len, alignment);

    return 0;
}
