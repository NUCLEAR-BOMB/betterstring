#include <betterstring/functions.hpp>
#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

namespace {

#if 0

TEST_F(char_traits, std_string) {
    using string = std::basic_string<char, bs::char_traits<char>>;
    string str;
    str.resize(5, ' ');
    EXPECT_EQ(str, "     ");
    EXPECT_EQ(str[0], ' ');
    EXPECT_EQ(str.at(1), ' ');
    str[2] = 'j';
    EXPECT_EQ(str.find('j'), 2);
    const auto str2 = str.substr(2);
    EXPECT_EQ(str2, "j  ");
}

TEST_F(char_traits, std_string_view) {
    using string_view = std::basic_string_view<char, bs::char_traits<char>>;

    const string_view str("test test test");
    EXPECT_EQ(str[0], 't');
    EXPECT_EQ(str[13], 't');
    EXPECT_EQ(str.size(), 14);
    EXPECT_EQ(str.find('e'), 1);
    EXPECT_EQ(str.find(" test"), 4);
}

#endif

}
