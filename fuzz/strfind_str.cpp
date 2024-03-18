#include <cinttypes>
#include <cassert>

#include <betterstring/functions.hpp>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
    if (Size < 2) { return -1; }

    const size_t needle_size = *reinterpret_cast<const uint16_t*>(Data);
    if (needle_size > Size) { return -1; }

    const auto needle = reinterpret_cast<const char*>(Data) + 2;

    const size_t haystack_size = Size - 2 - needle_size;
    const auto haystack = reinterpret_cast<const char*>(Data) + 2 + needle_size;

    if (needle_size > 32) { return -1; }
    if (haystack_size > 96) { return -1; }

    const auto result = bs::strfind(haystack, haystack_size, needle, needle_size);

    if (result == nullptr) {
        for (std::size_t i = 0; i + needle_size < haystack_size; ++i) {
            if (haystack[i] == needle[0] && std::memcmp(haystack, needle, needle_size) == 0) {
                std::abort();
            }
        }
    } else {
        if (result > haystack + haystack_size - needle_size) {
            std::abort();
        }

        for (auto it = haystack; it != result; ++it) {
            if (*it == needle[0] && std::memcmp(it, needle, needle_size) == 0) {
                std::abort();
            }
        }
    }

    return 0;
}
