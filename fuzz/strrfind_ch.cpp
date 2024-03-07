#include <cinttypes>
#include <cassert>

#include <betterstring/functions.hpp>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
    if (Size == 0) return -1;

    const char search_for = reinterpret_cast<const char&>(Data[0]);
    const char* const search_str = reinterpret_cast<const char*>(Data + 1);
    const std::size_t search_str_len = Size - 1;

    const char* result = bs::strrfind(search_str, search_str_len, search_for);

    if (result + 1 == nullptr) {
        for (std::size_t i = search_str_len; i != 0; --i) {
            if (search_str[i - 1] == search_for) {
                std::abort();
            }
        }
    } else {
        if (*result != search_for) std::abort();

        for (auto it = search_str + search_str_len - 1; it != result; --it) {
            if (*it == search_for) std::abort();
        }
    }

    return 0;
}
