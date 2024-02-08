#include <cstdint>
#include <cstring>

#include <betterstring/functions.hpp>

std::size_t simple_strcount(const char* str, std::size_t str_len, char ch) {
    std::size_t result = 0;
    for (std::size_t i = 0; i < str_len; ++i) {
        if (str[i] == ch) { ++result; }
    }
    return result;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
    if (Size == 0) return -1;

    const char char_to_count = reinterpret_cast<const char&>(Data[0]);
    const char* const str = reinterpret_cast<const char*>(Data + 1);
    const std::size_t str_len = Size - 1;

    const std::size_t result = bs::strcount(str, str_len, char_to_count);

    if (result == 0) {
        const void* match = std::memchr(str, static_cast<unsigned char>(char_to_count), str_len);
        if (match != nullptr) {
            std::abort();
        }
    }
    
    const std::size_t true_result = simple_strcount(str, str_len, char_to_count);

    if (result != true_result) {
        std::abort();
    }

    return 0;
}
