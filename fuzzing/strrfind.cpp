#include <cinttypes>
#include <cassert>

#ifndef __AVX2__
#error Need AVX2
#endif

#include <betterstring.hpp>

static const char* simple_strrfind(const char* const s, const std::size_t count, const char ch) {
    for (std::size_t i = count; i != 0; --i) {
        if (s[i - 1] == ch) return &s[i - 1];
    }
    return nullptr;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
    if (Size == 0) return -1;

    const char search_for = reinterpret_cast<const char&>(Data[0]);
    const char* const search_str = reinterpret_cast<const char*>(Data + 1);
    const std::size_t search_str_len = Size - 1;

    const char* result = bs::strrfind(search_str, search_str_len, search_for);
    const char* true_result = simple_strrfind(search_str, search_str_len, search_for);
    assert(result == true_result);

    return 0;
}
