#include <betterstring/functions.hpp>
#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

namespace {

TEST_CASE("std::string", "[char_traits]") {
    using string = std::basic_string<char, bs::char_traits<char>>;
    string str;
    str.resize(5, ' ');
    CHECK(str == "     ");
    CHECK(str[0] == ' ');
    CHECK(str.at(1) == ' ');
    str[2] = 'j';
    CHECK(str.find('j') == 2);
    const auto str2 = str.substr(2);
    CHECK(str2 == "j  ");
}

TEST_CASE("std::string_view", "[char_traits]") {
    using string_view = std::basic_string_view<char, bs::char_traits<char>>;

    const string_view str("test test test");
    CHECK(str[0] == 't');
    CHECK(str[13] == 't');
    CHECK(str.size() == 14);
    CHECK(str.find('e') == 1);
    CHECK(str.find(" test") == 4);
}

}
