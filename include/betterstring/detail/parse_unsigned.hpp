#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <algorithm>

#include <betterstring/detail/preprocessor.hpp>
#include <betterstring/type_traits.hpp>

#if BS_COMP_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wsign-conversion"
#elif BS_COMP_MSVC
    #pragma warning(push)
    #pragma warning(disable : 4365)
#endif

namespace bs {

// 'invalid_argument' should be equal to 1 because the compiler can use the 'set*'
// instructions instead of using the 'cmov*' instructions to specify an invalid argument error
// Example:
//     uint8_t digit = str[0] - '0';
//     value = digit;
//     return digit > 9 ? invalid_string : parse_error{};
// This code should be compiled into similar following assembly instructions:
//     movzx   r8d, BYTE PTR [rdx]
//     sub     r8b, 48
//     mov     BYTE PTR [rcx], r8b
//     xor     eax, eax
//     cmp     r8b, 9
//     seta    al
//     ret
enum class parse_error {
    invalid_argument = 1,
    out_of_range = 2,
    too_long = 3,
};

}

namespace bs::detail {

template<class T, class Ch>
BS_FORCEINLINE parse_error swar_parse_unsigned2(T& value, const Ch* const str) {
    uint16_t chunk;
    std::memcpy(&chunk, str, 2);

    const bool is_valid_string = (chunk & 0xF0F0) == 0x3030 && ((chunk + 0x0606) & 0xF0F0) == 0x3030;
    if (!is_valid_string) { return parse_error::invalid_argument; }

    chunk = ((chunk & 0x0F00) >> 8) + (chunk & 0x000F) * 10;

    value = static_cast<T>(chunk);

    return parse_error{};
}

template<class T, class Ch>
BS_FORCEINLINE parse_error swar_parse_unsigned4(T& value, const Ch* const str) {
    uint32_t chunk;
    std::memcpy(&chunk, str, 4);

    const bool is_valid_string = (chunk & 0xF0F0F0F0) == 0x30303030
                              && ((chunk + 0x06060606) & 0xF0F0F0F0) == 0x30303030;
    if (!is_valid_string) { return parse_error::invalid_argument; }

    chunk = ((chunk & 0x0F000F00) >> 8) + (chunk & 0x000F000F) * 10;
    chunk = ((chunk & 0x00FF0000) >> 16) + (chunk & 0x000000FF) * 100;

    value = static_cast<T>(chunk);

    return parse_error{};
}

template<class T, class Ch>
BS_FORCEINLINE parse_error swar_parse_unsigned8(T& value, const Ch* const str) {
    uint64_t chunk;
    std::memcpy(&chunk, str, 8);

    // Each digit is in ASCII ('0'..'9') has the representing values in the range 0x30..0x39.
    // So, we use bitwise AND with 0xF0F0F0F0F0F0F0F0 to get the higher part of each byte.
    // Then we compare it to 0x3030303030303030 to check if the higher part of each byte is 0x3.
    // If this statement is true, then we know that every byte in the string is between 0x30 and 0x39.
    // For example: the ASCII character '!' (0x21) will not successed because his higher part is equal to 0x2,
    // but character '3' (0x33) will pass because its higher part is 0x3.
    //
    // Next, we add the value 0x0606060606060606.
    // If a byte contains a valid digit character, it will never "overflow" into the higher part:
    // '0' (0x30) + 0x06 = 0x36, '9' (0x39) + 0x06 = 0x3F.
    // But if the value is greater than 9, it will "overflow" to the higher part of the byte:
    // ':' (0x3A) + 0x06 = 0x40, '?' (0x3F) + 0x06 = 0x45.
    // Then we perform a bitwise AND with 0xF0F0F0F0F0F0F0F0 and compare its result with 0x3030303030303030
    // to check if the lower part of a byte is not "overflowed" to the higher part after adding 0x0606060606060606.
    //
    // This is equivalent to performing an operation the >= '0' && <= '9' for each byte,
    // i.e. checking whether each ASCII character of the string is a valid digit.
    const bool is_valid_string = (chunk & 0xF0F0F0F0F0F0F0F0) == 0x3030303030303030
                              && ((chunk + 0x0606060606060606) & 0xF0F0F0F0F0F0F0F0) == 0x3030303030303030;
    if (!is_valid_string) { return parse_error::invalid_argument; }

    // The bytes in the chunk variable are arranged in the opposite order than in the original string,
    // if the current platform has little endianness i.e. "1234" -> 0x34333231. 

    // We do not need to subtract '0' (0x3030303030303030) from the chunk,
    // since it will be masked away. Example:
    // 0x35363738 ("8765") & 0xF0F0F0F = 0x05060708

    // Mask the chunk with 0x0F000F000F000F00 to get odd bytes,
    // and mask with 0x000F000F000F000F to get even bytes.
    // A shift is performed here because the odd bytes must match the even bytes
    // which are multiplied by 10 for further addition.
    // For example:
    // 
    // chunk = 0x0908070605040302 ("23456789")
    // 
    // chunk & 0x0F000F000F000F00 = 0x0900070005000300
    // chunk & 0x000F000F000F000F = 0x0008000600040002
    //
    // 0x0900070005000300 >> 8 = 0x0009000700050003
    // 0x0008000600040002 * 10 = 0x0050003C00280014
    //
    // 0x0009000700050003 + 0x0050003C00280014 = 0x00590043002D0017 (89 67 45 23)
    chunk = ((chunk & 0x0F000F000F000F00) >> 8) + (chunk & 0x000F000F000F000F) * 10;

    // Next steps are similar to first one.
    //
    // chunk = 0x00590043002D0017
    //
    // chunk & 0x00FF000000FF0000 = 0x00590000002D0000
    // chunk & 0x000000FF000000FF = 0x0000004300000017
    //
    // 0x00590000002D0000 >> 16 = 0x000000590000002D
    // 0x0000004300000017 * 100 = 0x00001A2C000008FC
    //
    // 0x000000590000002D + 0x00001A2C000008FC = 0x00001A8500000929 (6789 2345)
    chunk = ((chunk & 0x00FF000000FF0000) >> 16) + (chunk & 0x000000FF000000FF) * 100;

    // chunk = 0x00001A8500000929
    //
    // chunk & 0x0000FFFF00000000 = 0x00001A8500000000
    // chunk & 0x000000000000FFFF = 0x0000000000000929
    //
    // 0x00001A8500000000 >> 32 = 0x0000000000001A85
    // 0x0000000000000929 * 10000 = 0x000000000165D190
    //
    // 0x0000000000001A85 + 0x000000000165D190 = 0x000000000165EC15 (23456789)
    chunk = ((chunk & 0x0000FFFF00000000) >> 32) + (chunk & 0x000000000000FFFF) * 10000;

    value = static_cast<T>(chunk);

    return parse_error{};
}

template<class T, class Ch>
BS_FORCEINLINE parse_error simple_parse_unsigned(T& value, const Ch* const str, const std::size_t count) {
    T tmp = 0;
    for (std::size_t i = 0; i < count; ++i) {
        T digit = str[i] - '0';
        if (digit > 9) { return parse_error::invalid_argument; }
        tmp = tmp * 10 + digit;
    }
    value = tmp;
    return parse_error{};
}

template<class T, class Ch>
constexpr parse_error constexpr_parse_unsigned(T& value, const Ch* const str, const std::size_t count) noexcept {
    T tmp = 0;
    for (auto it = str; it != str + count; ++it) {
        T digit = static_cast<T>(*it - '0');
        if (digit > 9) { return parse_error::invalid_argument; }

        constexpr T max_val = T(-1) / 10;
        constexpr T max_digit = T(-1) % 10;

        if (tmp < max_val || (tmp == max_val && digit <= max_digit)) {
            tmp = tmp * 10 + digit;
        } else {
            return parse_error::out_of_range;
        }
    }
    value = tmp;
    return parse_error{};
}

template<class T, class Ch, std::enable_if_t<sizeof(T) == 1, int> = 0>
parse_error parse_unsigned(T& value, const Ch* const str, const std::size_t count) {
    constexpr parse_error invalid_string = parse_error::invalid_argument;
    switch (count)
    {
    case 0: return invalid_string;
    case 1: {
        // uint8_t(str[0] - '0') > 9 is same as (str[0] - '0') >= '0' && (str[0] - '0') <= '9'.
        uint8_t digit = str[0] - '0';
        value = digit;
        return digit > 9 ? invalid_string : parse_error{};
    }
    case 2: {
#if 1
        return swar_parse_unsigned2(value, str);
#else
        uint8_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint8_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        value = digit0 * 10 + digit1;
        return parse_error{};
#endif
    }
    case 3: {
        uint8_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint8_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint8_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint16_t sum = (digit0 * 10 + digit1) * 10 + digit2;
        if (sum > 255) { return parse_error::out_of_range; }
        value = static_cast<T>(sum);
        return parse_error{};
    }
    default:
        return parse_error::too_long;
    }
}

template<class T, class Ch, std::enable_if_t<sizeof(T) == 2, int> = 0>
parse_error parse_unsigned(T& value, const Ch* const str, const std::size_t count) {
    constexpr parse_error invalid_string = parse_error::invalid_argument;
    switch (count)
    {
    case 0: return invalid_string;
    case 1: {
        uint16_t digit = str[0] - '0';
        value = digit;
        return digit > 9 ? invalid_string : parse_error{};
    }
    case 2: {
#if 1
        return swar_parse_unsigned2(value, str);
#else
        uint16_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint16_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        value = digit0 * 10 + digit1;
        return parse_error{};
#endif
    }
    case 3: {
        uint16_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint16_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint16_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        value = (digit0 * 10 + digit1) * 10 + digit2;
        return parse_error{};
    }
    case 4: {
#if 1
        return swar_parse_unsigned4(value, str);
#else
        uint16_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint16_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint16_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint16_t digit3 = str[3] - '0';
        if (digit3 > 9) { return invalid_string; }
        value = ((digit0 * 10 + digit1) * 10 + digit2) * 10 + digit3;
        return parse_error{};
#endif
    }
    case 5: {
        uint32_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint32_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint32_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint32_t digit3 = str[3] - '0';
        if (digit3 > 9) { return invalid_string; }
        uint32_t digit4 = str[4] - '0';
        if (digit4 > 9) { return invalid_string; }
        uint32_t sum = (((digit0 * 10 + digit1) * 10 + digit2) * 10 + digit3) * 10 + digit4;
        //uint32_t sum = digit0 * 10000 + digit1 * 1000 + digit2 * 100 + digit3 * 10 + digit4;
        if (sum > 65535) { return parse_error::out_of_range; }
        value = static_cast<T>(sum);
        return parse_error{};
    }
    default:
        return parse_error::too_long;
    }
}

template<class T, class Ch, std::enable_if_t<sizeof(T) == 4, int> = 0>
parse_error parse_unsigned(T& value, const Ch* const str, const std::size_t count) {
    constexpr parse_error invalid_string = parse_error::invalid_argument;
    switch (count)
    {
    case 0: return invalid_string;
    case 1: {
        uint32_t digit = str[0] - '0';
        value = digit;
        return digit > 9 ? invalid_string : parse_error{};
    }
    case 2: {
#if 1
        return swar_parse_unsigned2(value, str);
#else
        uint32_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint32_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        value = digit0 * 10 + digit1;
        return parse_error{};
#endif
    }
    case 3: {
        uint32_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint32_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint32_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        value = (digit0 * 10 + digit1) * 10 + digit2;
        return parse_error{};
    }
    case 4: {
#if 1
        return swar_parse_unsigned4(value, str);
#else
        uint32_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint32_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint32_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint32_t digit3 = str[3] - '0';
        if (digit3 > 9) { return invalid_string; }
        value = ((digit0 * 10 + digit1) * 10 + digit2) * 10 + digit3;
        return parse_error{};
#endif
    }
    case 5: {
        uint32_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint32_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint32_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint32_t digit3 = str[3] - '0';
        if (digit3 > 9) { return invalid_string; }
        uint32_t digit4 = str[4] - '0';
        if (digit4 > 9) { return invalid_string; }
        value = (((digit0 * 10 + digit1) * 10 + digit2) * 10 + digit3) * 10 + digit4;
        return parse_error{};
    }
    case 6: {
        uint32_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint32_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint32_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint32_t digit3 = str[3] - '0';
        if (digit3 > 9) { return invalid_string; }
        uint32_t digit4 = str[4] - '0';
        if (digit4 > 9) { return invalid_string; }
        uint32_t digit5 = str[5] - '0';
        if (digit5 > 9) { return invalid_string; }
        value = ((((digit0 * 10 + digit1) * 10 + digit2) * 10 + digit3) * 10 + digit4) * 10 + digit5;
        return parse_error{};
    }
    case 7: {
        uint32_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint32_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint32_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint32_t digit3 = str[3] - '0';
        if (digit3 > 9) { return invalid_string; }
        uint32_t digit4 = str[4] - '0';
        if (digit4 > 9) { return invalid_string; }
        uint32_t digit5 = str[5] - '0';
        if (digit5 > 9) { return invalid_string; }
        uint32_t digit6 = str[6] - '0';
        if (digit6 > 9) { return invalid_string; }
        value = (((((digit0 * 10 + digit1) * 10 + digit2) * 10 + digit3) * 10 + digit4) * 10 + digit5) * 10 + digit6;
        return parse_error{};
    }
    case 8: {
#if 1
        return swar_parse_unsigned8(value, str);
#else
        uint32_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint32_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint32_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint32_t digit3 = str[3] - '0';
        if (digit3 > 9) { return invalid_string; }
        uint32_t digit4 = str[4] - '0';
        if (digit4 > 9) { return invalid_string; }
        uint32_t digit5 = str[5] - '0';
        if (digit5 > 9) { return invalid_string; }
        uint32_t digit6 = str[6] - '0';
        if (digit6 > 9) { return invalid_string; }
        uint32_t digit7 = str[7] - '0';
        if (digit7 > 9) { return invalid_string; }
        value = ((((((digit0 * 10 + digit1) * 10 + digit2) * 10 + digit3) * 10 + digit4) * 10 + digit5) * 10 + digit6) * 10 + digit7;
        return parse_error{};
#endif
    }
    case 9: {
        uint32_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint32_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint32_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint32_t digit3 = str[3] - '0';
        if (digit3 > 9) { return invalid_string; }
        uint32_t digit4 = str[4] - '0';
        if (digit4 > 9) { return invalid_string; }
        uint32_t digit5 = str[5] - '0';
        if (digit5 > 9) { return invalid_string; }
        uint32_t digit6 = str[6] - '0';
        if (digit6 > 9) { return invalid_string; }
        uint32_t digit7 = str[7] - '0';
        if (digit7 > 9) { return invalid_string; }
        uint32_t digit8 = str[8] - '0';
        if (digit8 > 9) { return invalid_string; }
        value = (((((((digit0 * 10 + digit1) * 10 + digit2) * 10 + digit3) * 10 + digit4) * 10 + digit5) * 10 + digit6) * 10 + digit7) * 10 + digit8;
        return parse_error{};
    }
    case 10: {
        uint32_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint32_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint32_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint32_t digit3 = str[3] - '0';
        if (digit3 > 9) { return invalid_string; }
        uint32_t digit4 = str[4] - '0';
        if (digit4 > 9) { return invalid_string; }
        uint32_t digit5 = str[5] - '0';
        if (digit5 > 9) { return invalid_string; }
        uint32_t digit6 = str[6] - '0';
        if (digit6 > 9) { return invalid_string; }
        uint32_t digit7 = str[7] - '0';
        if (digit7 > 9) { return invalid_string; }
        uint32_t digit8 = str[8] - '0';
        if (digit8 > 9) { return invalid_string; }
        uint32_t digit9 = str[9] - '0';
        if (digit9 > 9) { return invalid_string; }
        uint64_t sum = ((((((((digit0 * 10ULL + digit1) * 10ULL + digit2) * 10ULL + digit3) * 10ULL + digit4) * 10ULL + digit5) * 10ULL + digit6) * 10ULL + digit7) * 10ULL + digit8) * 10ULL + digit9;
        if (sum > 4294967295) { return parse_error::out_of_range; }
        value = static_cast<T>(sum);
        return parse_error{};
    }
    default:
        return parse_error::too_long;
    }
}

template<class T, class Ch, std::enable_if_t<sizeof(T) == 8, int> = 0>
parse_error parse_unsigned(T& value, const Ch* const str, const std::size_t count) {
    constexpr parse_error invalid_string = parse_error::invalid_argument;
    switch (count)
    {
    case 0: return parse_error::invalid_argument;
    case 1: {
        uint64_t digit = str[0] - '0';
        value = digit;
        return digit > 9 ? invalid_string : parse_error{};
    }
    case 2: {
        return swar_parse_unsigned2(value, str);
    }
    case 3: {
        uint64_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint64_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint64_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        value = (digit0 * 10 + digit1) * 10 + digit2;
        return parse_error{};
    }
    case 4: {
        return swar_parse_unsigned4(value, str);
    }
    case 5: {
        uint64_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint64_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint64_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint64_t digit3 = str[3] - '0';
        if (digit3 > 9) { return invalid_string; }
        uint64_t digit4 = str[4] - '0';
        if (digit4 > 9) { return invalid_string; }
        value = (((digit0 * 10 + digit1) * 10 + digit2) * 10 + digit3) * 10 + digit4;
        return parse_error{};
    }
    case 6: {
        uint64_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint64_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint64_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint64_t digit3 = str[3] - '0';
        if (digit3 > 9) { return invalid_string; }
        uint64_t digit4 = str[4] - '0';
        if (digit4 > 9) { return invalid_string; }
        uint64_t digit5 = str[5] - '0';
        if (digit5 > 9) { return invalid_string; }
        value = ((((digit0 * 10 + digit1) * 10 + digit2) * 10 + digit3) * 10 + digit4) * 10 + digit5;
        return parse_error{};
    }
    case 7: {
        uint64_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint64_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint64_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint64_t digit3 = str[3] - '0';
        if (digit3 > 9) { return invalid_string; }
        uint64_t digit4 = str[4] - '0';
        if (digit4 > 9) { return invalid_string; }
        uint64_t digit5 = str[5] - '0';
        if (digit5 > 9) { return invalid_string; }
        uint64_t digit6 = str[6] - '0';
        if (digit6 > 9) { return invalid_string; }
        value = (((((digit0 * 10 + digit1) * 10 + digit2) * 10 + digit3) * 10 + digit4) * 10 + digit5) * 10 + digit6;
        return parse_error{};
    }
    case 8: {
        return swar_parse_unsigned8(value, str);
    }
    case 9: {
        uint64_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint64_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint64_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint64_t digit3 = str[3] - '0';
        if (digit3 > 9) { return invalid_string; }
        uint64_t digit4 = str[4] - '0';
        if (digit4 > 9) { return invalid_string; }
        uint64_t digit5 = str[5] - '0';
        if (digit5 > 9) { return invalid_string; }
        uint64_t digit6 = str[6] - '0';
        if (digit6 > 9) { return invalid_string; }
        uint64_t digit7 = str[7] - '0';
        if (digit7 > 9) { return invalid_string; }
        uint64_t digit8 = str[8] - '0';
        if (digit8 > 9) { return invalid_string; }
        value = (((((((digit0 * 10 + digit1) * 10 + digit2) * 10 + digit3) * 10 + digit4) * 10 + digit5) * 10 + digit6) * 10 + digit7) * 10 + digit8;
        return parse_error{};
    }
    case 10: {
        uint64_t digit0 = str[0] - '0';
        if (digit0 > 9) { return invalid_string; }
        uint64_t digit1 = str[1] - '0';
        if (digit1 > 9) { return invalid_string; }
        uint64_t digit2 = str[2] - '0';
        if (digit2 > 9) { return invalid_string; }
        uint64_t digit3 = str[3] - '0';
        if (digit3 > 9) { return invalid_string; }
        uint64_t digit4 = str[4] - '0';
        if (digit4 > 9) { return invalid_string; }
        uint64_t digit5 = str[5] - '0';
        if (digit5 > 9) { return invalid_string; }
        uint64_t digit6 = str[6] - '0';
        if (digit6 > 9) { return invalid_string; }
        uint64_t digit7 = str[7] - '0';
        if (digit7 > 9) { return invalid_string; }
        uint64_t digit8 = str[8] - '0';
        if (digit8 > 9) { return invalid_string; }
        uint64_t digit9 = str[9] - '0';
        if (digit9 > 9) { return invalid_string; }
        value = ((((((((digit0 * 10 + digit1) * 10 + digit2) * 10 + digit3) * 10 + digit4) * 10 + digit5) * 10 + digit6) * 10 + digit7) * 10 + digit8) * 10 + digit9;
        return parse_error{};
    }
    case 16: {
        uint64_t val1;
        parse_error err1 = swar_parse_unsigned8(val1, str);
        if (err1 == invalid_string) { return invalid_string; }
        uint64_t val2;
        parse_error err2 = swar_parse_unsigned8(val2, str + 8);
        if (err2 == invalid_string) { return invalid_string; }
        value = val1 * 100000000 + val2;
        return parse_error{};
    }
    case 20: {
        uint64_t val;
        parse_error err = simple_parse_unsigned(val, str, 19);
        if (err == invalid_string) { return invalid_string; }

        uint64_t digit = str[19] - '0';
        if (digit > 9) { return invalid_string; }

        constexpr uint64_t max_val = uint64_t(-1) / 10;
        constexpr uint64_t max_digit = uint64_t(-1) % 10;

        if (val >= max_val && (val != max_val || digit > max_digit)) {
            return parse_error::out_of_range;
        }
        value = static_cast<T>(val * 10 + digit);
        return parse_error{};
    }

    default:
        if (count < 20) {
            return simple_parse_unsigned(value, str, count);
        }
        return parse_error::too_long;
    }
}

}

#if BS_COMP_CLANG
    #pragma clang diagnostic pop
#elif BS_COMP_MSVC
    #pragma warning(pop)
#endif
