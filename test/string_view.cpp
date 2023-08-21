#include <gtest/gtest.h>

#include <betterstring.hpp>

namespace {

struct string_view : ::testing::Test {
    bs::string_view<> str{"test string"};
    const bs::string_view<> const_str{"test string"};
};

TEST_F(string_view, constructor) {
    bs::string_view<> empty_str;
    EXPECT_EQ(empty_str.size(), 0);
    EXPECT_EQ(empty_str.data(), nullptr);

    bs::string_view<> copy_str(str);
    EXPECT_EQ(copy_str, str);

    bs::string_view<> cut_str("test string", 4);
    EXPECT_EQ(cut_str, "test");

    bs::string_view<> cstr_view("test str");
    EXPECT_EQ(cstr_view, "test str");

    constexpr auto cstring = "123456789";
    bs::string_view<> range_str(cstring + 1, cstring + 7);
    EXPECT_EQ(range_str, "234567");
}

TEST_F(string_view, begin_end) {
    (void)str.begin();
    (void)str.cbegin();
    (void)str.crbegin();

    (void)str.end();
    (void)str.cend();
    (void)str.crend();
}

TEST_F(string_view, subscript) {
    EXPECT_EQ(str[0], 't');
    EXPECT_EQ(str[10], 'g');
}

}
