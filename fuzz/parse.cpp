#include <cstdint>

#include <betterstring/parsing.hpp>
#include <charconv>

template<class T>
void parse_fuzz(const char* const str, const std::size_t count) {
    const auto res = bs::parse<T>(str, count);

    T fc_res = 0;
    const auto fc_err = std::from_chars(str, str + count, fc_res).ec;
    if (res.error() != bs::parse_error::too_long) {
        if (fc_err == std::errc::invalid_argument && res.error() != bs::parse_error::invalid_argument) {
            std::abort();
        }
        if (fc_err == std::errc::result_out_of_range && res.error() != bs::parse_error::out_of_range) {
            std::abort();
        }
    }
    if (!res.has_error()) {
        if (res.value() != fc_res) {
            std::abort();
        }
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
