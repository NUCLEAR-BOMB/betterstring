#include <cinttypes>
#include <cassert>

#ifndef __AVX2__
#error Need AVX2
#endif

#include <betterstring.hpp>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
    if (Size < 1) return -1;
    const size_t needle_size = Data[0];
    if (needle_size >= Size) return -1;
    const auto needle = reinterpret_cast<const char*>(Data) + 1;

    const size_t haystack_size = Size - 1 - needle_size;
    const auto haystack = reinterpret_cast<const char*>(Data) + 1 + needle_size;

    const auto result = bs::strrfind(haystack, haystack_size, needle, needle_size);

    if (result == nullptr) {
        for (std::size_t i = 0; i + needle_size < haystack_size; ++i) {
            if (std::memcmp(needle, haystack + i, needle_size) == 0) {
                bs::strrfind(haystack, haystack_size, needle, needle_size);
                std::abort();
            }
        }
    } else {
        if (std::memcmp(needle, result, needle_size) != 0) {
            std::abort();
        }
        for (auto it = haystack + (haystack_size - needle_size); it != result; --it) {
            if (std::memcmp(it, needle, needle_size) == 0) {
                bs::strrfind(haystack, haystack_size, needle, needle_size);
                std::abort();
            }
        }
    }

    return 0;
}
