#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <betterstring/parsing.hpp>

#if BS_COMP_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wimplicitly-unsigned-literal"
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

static_assert(static_cast<uint8_t>(bs::parse_error{}) == 0);
static_assert(static_cast<uint8_t>(bs::parse_error::invalid_argument) == 1);

using std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t;

namespace Catch {
    template<class T>
    struct StringMaker<bs::parse_result<T>> {
        static std::string convert(const bs::parse_result<T>& value) {
            if (value.has_error()) { return "[error]: " + std::to_string(uint8_t(value.error())); }
            return std::to_string(value.value());
        }
    };
}

namespace {

TEST_CASE("uint8", "[parsing]") {
    CHECK(bs::parse<uint8_t>("", 0) == bs::parse_error::invalid_argument);
    CHECK(bs::parse<uint8_t>("1", 1) == 1);
    CHECK(bs::parse<uint8_t>("a", 1) == bs::parse_error::invalid_argument);
    CHECK(bs::parse<uint8_t>("10", 2) == 10);
    CHECK(bs::parse<uint8_t>("12", 2) == 12);
    CHECK(bs::parse<uint8_t>("1b", 2) == bs::parse_error::invalid_argument);
    CHECK(bs::parse<uint8_t>("123", 3) == 123);
    CHECK(bs::parse<uint8_t>("255", 3) == 255);
    CHECK(bs::parse<uint8_t>("256", 3) == bs::parse_error::out_of_range);
    CHECK(bs::parse<uint8_t>("999", 3) == bs::parse_error::out_of_range);
    CHECK(bs::parse<uint8_t>("14b", 3) == bs::parse_error::invalid_argument);
    CHECK(bs::parse<uint8_t>("1234", 4) == bs::parse_error::too_long);

    if (auto val = bs::parse<uint8_t>("123", 3)) {
        CHECK(val == 123);
    } else {
        FAIL();
    }
    if (auto val = bs::parse<uint8_t>("a", 1)) {
        FAIL();
    } else {
        CHECK(val.error() == bs::parse_error::invalid_argument);
    }
}

TEST_CASE("uint16", "[parsing]") {
    CHECK(bs::parse<uint16_t>("", 0) == bs::parse_error::invalid_argument);
    CHECK(bs::parse<uint16_t>("1", 1) == 1);
    CHECK(bs::parse<uint16_t>("a", 1) == bs::parse_error::invalid_argument);
    CHECK(bs::parse<uint16_t>("10", 2) == 10);
    CHECK(bs::parse<uint16_t>("ab", 2) == bs::parse_error::invalid_argument);
    CHECK(bs::parse<uint16_t>("546", 3) == 546);
    CHECK(bs::parse<uint16_t>("1234", 4) == 1234);
    CHECK(bs::parse<uint16_t>("65535", 5) == 65535);
    CHECK(bs::parse<uint16_t>("65536", 5) == bs::parse_error::out_of_range);
    CHECK(bs::parse<uint16_t>("123456", 6) == bs::parse_error::too_long);
}

TEST_CASE("uint32", "[parsing]") {
    CHECK(bs::parse<uint32_t>("", 0) == bs::parse_error::invalid_argument);
    CHECK(bs::parse<uint32_t>("2", 1) == 2);
    CHECK(bs::parse<uint32_t>("12", 2) == 12);
    CHECK(bs::parse<uint32_t>("444", 3) == 444);
    CHECK(bs::parse<uint32_t>("9999", 4) == 9999);
    CHECK(bs::parse<uint32_t>("12345", 5) == 12345);
    CHECK(bs::parse<uint32_t>("101010", 6) == 101010);
    CHECK(bs::parse<uint32_t>("5040302", 7) == 5040302);
    CHECK(bs::parse<uint32_t>("11111111", 8) == 11111111);
    CHECK(bs::parse<uint32_t>("84848484", 8) == 84848484);
    CHECK(bs::parse<uint32_t>("100200300", 9) == 100200300);
    CHECK(bs::parse<uint32_t>("2000000000", 10) == 2000000000U);
    CHECK(bs::parse<uint32_t>("4294967295", 10) == 4294967295U);
    CHECK(bs::parse<uint32_t>("4294967296", 10) == bs::parse_error::out_of_range);
    CHECK(bs::parse<uint32_t>("10000000000", 11) == bs::parse_error::too_long);

    static constexpr bs::parse_result constexpr_val1 = bs::parse<uint32_t>("123", 3);
    CHECK(constexpr_val1 == 123);
    static constexpr bs::parse_result constexpr_val2 = bs::parse<uint32_t>("aa", 2);
    CHECK(constexpr_val2 == bs::parse_error::invalid_argument);
    static constexpr bs::parse_result constexpr_val3 = bs::parse<uint32_t>("133278947392427621", 18);
    CHECK(constexpr_val3 == bs::parse_error::out_of_range);
}

TEST_CASE("uint64", "[parsing]") {
    CHECK(bs::parse<uint64_t>("", 0) == bs::parse_error::invalid_argument);
    CHECK(bs::parse<uint64_t>("1", 1) == 1);
    CHECK(bs::parse<uint64_t>("12", 2) == 12);
    CHECK(bs::parse<uint64_t>("100", 3) == 100);
    CHECK(bs::parse<uint64_t>("9999", 4) == 9999);
    CHECK(bs::parse<uint64_t>("88111", 5) == 88111);
    CHECK(bs::parse<uint64_t>("101010", 6) == 101010);
    CHECK(bs::parse<uint64_t>("1234567", 7) == 1234567);
    CHECK(bs::parse<uint64_t>("11111111", 8) == 11111111);
    CHECK(bs::parse<uint64_t>("282828282", 9) == 282828282);
    CHECK(bs::parse<uint64_t>("1919191919", 10) == 1919191919);
    CHECK(bs::parse<uint64_t>("67676767676", 11) == 67676767676);
    CHECK(bs::parse<uint64_t>("499499499499", 12) == 499499499499);
    CHECK(bs::parse<uint64_t>("1002003004005", 13) == 1002003004005);
    CHECK(bs::parse<uint64_t>("98798798798798", 14) == 98798798798798);
    CHECK(bs::parse<uint64_t>("440044004400440", 15) == 440044004400440);
    CHECK(bs::parse<uint64_t>("8627337531537851", 16) == 8627337531537851);
    CHECK(bs::parse<uint64_t>("75959909444697342", 17) == 75959909444697342);
    CHECK(bs::parse<uint64_t>("197206569500019260", 18) == 197206569500019260);
    CHECK(bs::parse<uint64_t>("9695451996318117111", 19) == 9695451996318117111);

    CHECK(bs::parse<uint64_t>("10000000000000000002", 20) == 10000000000000000002);
    CHECK(bs::parse<uint64_t>("18446744073709551615", 20) == 18446744073709551615);

    CHECK(bs::parse<uint64_t>("18446744073709560000", 20) == bs::parse_error::out_of_range);
    CHECK(bs::parse<uint64_t>("18446744073709551616", 20) == bs::parse_error::out_of_range);
    CHECK(bs::parse<uint64_t>("18446744073709561616", 20) == bs::parse_error::out_of_range);

    CHECK(bs::parse<uint64_t>("18446744073709551616", 20) == bs::parse_error::out_of_range);
    CHECK(bs::parse<uint64_t>("18446744073709551625", 20) == bs::parse_error::out_of_range);

    CHECK(bs::parse<uint64_t>("143489732497312498241", 21) == bs::parse_error::too_long);
}

}

#if BS_COMP_CLANG
    #pragma clang diagnostic pop
#endif
