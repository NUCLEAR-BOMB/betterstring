#include <cstdint>

#include <betterstring/parsing.hpp>
#include <charconv>

#define TRAP() std::abort()

template<class T>
void parse_fuzz(const char* const str, const std::size_t count) {
    const auto res = bs::parse<T>(str, count);

    T fc_res = 0;
    const std::from_chars_result fc_err = std::from_chars(str, str + count, fc_res);

    if (res.error() == bs::parse_error::too_long) {
        return;
    }
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
    if (Size >= 25) return -1;

    const char* const str = reinterpret_cast<const char*>(Data);

    parse_fuzz<std::uint8_t>(str, Size);
    parse_fuzz<std::uint16_t>(str, Size);
    parse_fuzz<std::uint32_t>(str, Size);
    parse_fuzz<std::uint64_t>(str, Size);

    return 0;
}
