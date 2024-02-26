#include <cinttypes>
#include <cassert>
#include <new>

#include <betterstring/functions.hpp>
#include <algorithm>

static constexpr uint8_t max_needle_len = 6;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
    if (Size == 0) { return -1; }

    const size_t needle_len = (Data[0] % (max_needle_len + 1)) <= Size ? (Data[0] % (max_needle_len + 1)) : Size;

    const char* const needle = reinterpret_cast<const char*>(Data);

    const size_t str_len = Size - needle_len;

    const char* const str = reinterpret_cast<const char*>(Data + needle_len);

    const char* const result = bs::strfirst_of(str, str_len, needle, needle_len);

    const char* const true_result = std::find_first_of(str, str + str_len, needle, needle + needle_len);
    if (true_result == str + str_len) {
        if (result != nullptr) {
            std::abort();
        }
    } else {
        if (result != true_result) {
            std::abort();
        }
    }

    return 0;
}
