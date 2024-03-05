#include <cstdint>

#include <betterstring/parsing.hpp>
#include <charconv>

#define TRAP() std::abort()

template<class T>
void parse_fuzz(const char* const str, const std::size_t count) {
    const auto res = bs::parse<T>(str, count);

    T fc_res = 0;
    const std::from_chars_result fc_err = std::from_chars(str, str + count, fc_res);

    if (fc_err.ec == std::errc::invalid_argument) {
        if (res.error() != bs::parse_error::invalid_argument) {
            TRAP();
        }
        return;
    }
    if (fc_err.ec == std::errc::result_out_of_range) {
        if (res.error() != bs::parse_error::out_of_range) {
            TRAP();
        }
        return;
    }
    if (fc_err.ptr != (str + count)) {
        if (res.error() != bs::parse_error::invalid_argument) {
            TRAP();
        }
        return;
    }

    if (res.has_error()) {
        TRAP();
    }

    if (res.value() != fc_res) {
        TRAP();
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
    if (Size > 30) { return -1; }

    const char* str = reinterpret_cast<const char*>(Data);
    std::size_t str_len = Size;

    uint64_t fc_res = 0;
    const auto [ptr, err] = std::from_chars(str, str + str_len, fc_res);
    if (err != std::errc{} || ptr != (str + str_len)) { return -1; }

    for (; str_len > 0; ++str, --str_len) {
        if (*str != '0') { break; }
    }

    if (str_len <= 20) {
        parse_fuzz<std::uint64_t>(str, str_len);
    }
    if (str_len <= 10) {
        parse_fuzz<std::uint32_t>(str, str_len);
    }
    if (str_len <= 5) {
        parse_fuzz<std::uint16_t>(str, str_len);
    }
    if (str_len <= 3) {
        parse_fuzz<std::uint8_t>(str, str_len);
    }

    return 0;
}
