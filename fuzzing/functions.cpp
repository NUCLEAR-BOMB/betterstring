#include <cinttypes>
#include <betterstring.hpp>

#define CHECK(expr) if (expr) {} else BS_DEBUG_BREAK

template<class T>
struct simple_string {
    T* const data;
    const std::size_t size;

    ~simple_string() noexcept {
        delete[] data;
    }
};

template<class Ch>
simple_string<Ch> new_nt_string(const std::uint8_t* data, const std::size_t size) {
    const std::size_t result_size = size / sizeof(Ch);
    Ch* const result = new Ch[result_size + 1];

    std::memcpy(result, data, result_size);
    result[result_size] = '\0';

    return {result, result_size};
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, const size_t Size) {
    auto char_str1 = new_nt_string<char>(Data, Size);
    auto wchar_str1 = new_nt_string<wchar_t>(Data, Size);
    auto u16_str1 = new_nt_string<char16_t>(Data, Size);
    auto u32_str1 = new_nt_string<char32_t>(Data, Size);

    auto char_str2 = new_nt_string<char>(Data, Size);
    auto wchar_str2 = new_nt_string<wchar_t>(Data, Size);
    auto u16_str2 = new_nt_string<char16_t>(Data, Size);
    auto u32_str2 = new_nt_string<char32_t>(Data, Size);

    CHECK(bs::strlen(char_str1.data) <= char_str1.size);
    CHECK(bs::strlen(wchar_str1.data) <= wchar_str1.size);
    CHECK(bs::strlen(u16_str1.data) <= u16_str1.size);
    CHECK(bs::strlen(u32_str1.data) <= u32_str1.size);

    bs::strcopy(char_str2.data, char_str1.data, char_str1.size);
    bs::strcopy(wchar_str2.data, wchar_str1.data, wchar_str1.size);
    bs::strcopy(u16_str2.data, u16_str1.data, u16_str1.size);
    bs::strcopy(u32_str2.data, u32_str1.data, u32_str1.size);

    return 0;
}
