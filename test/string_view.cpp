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

TEST_F(string_view, front_back) {
    EXPECT_EQ(str.front(), 't');
    EXPECT_EQ(str.back(), 'g');
}

TEST_F(string_view, data) {
    const auto cstring = "hello world";
    bs::string_view strview(cstring);
    EXPECT_EQ(strview.data(), cstring);
}

TEST_F(string_view, size_length) {
    EXPECT_EQ(str.size(), 11);
    EXPECT_EQ(str.length(), str.size());
}

TEST_F(string_view, max_size) {
    (void)str.max_size();
}

TEST_F(string_view, empty) {
    EXPECT_FALSE(str.empty());
    EXPECT_TRUE(bs::string_view("").empty());
}

TEST_F(string_view, remove_prefix) {
    str.remove_prefix(4);
    EXPECT_EQ(str, " string");
}

TEST_F(string_view, remove_suffix) {
    str.remove_suffix(3);
    EXPECT_EQ(str, "test str");
}

TEST_F(string_view, slice) {
    EXPECT_EQ(str(0, 4), "test");
    EXPECT_EQ(str(0, 4).size(), 4);

    EXPECT_EQ(str(5, 11), "string");
    EXPECT_EQ(str(5, 11).size(), 6);
}

TEST_F(string_view, substr) {
    EXPECT_EQ(str.substr(6), "tring");
    EXPECT_EQ(str.substr(4, 3), " st");
}

TEST_F(string_view, starts_with) {
    EXPECT_TRUE(str.starts_with("test"));
    EXPECT_FALSE(bs::string_view("hello").starts_with("hello!"));

    EXPECT_TRUE(str.starts_with(str));

    EXPECT_TRUE(str.starts_with('t'));
    EXPECT_FALSE(str.starts_with('e'));
}

TEST_F(string_view, ends_with) {
    EXPECT_TRUE(str.ends_with("string"));
    EXPECT_FALSE(str.ends_with("strin"));

    EXPECT_TRUE(str.ends_with(str));

    EXPECT_TRUE(str.ends_with('g'));
    EXPECT_FALSE(str.ends_with('n'));
}

TEST_F(string_view, operators) {
    // ==
    EXPECT_TRUE(str == str);
    EXPECT_TRUE(str == "test string");
    EXPECT_FALSE(str == "hello");
    // !=
    EXPECT_FALSE(str != str);
    EXPECT_TRUE(str != "a");
    EXPECT_FALSE(str != "test string");
    // >
    EXPECT_FALSE(str > str);
    EXPECT_TRUE(str > "a");
    EXPECT_FALSE(str > "test string+");
    EXPECT_TRUE(str > "test strina");
    // >=
    EXPECT_TRUE(str >= str);
    EXPECT_TRUE(str >= "b");
    EXPECT_FALSE(str >= "test string+");
    EXPECT_TRUE(str >= "test strina");
    // <
    EXPECT_FALSE(str < str);
    EXPECT_TRUE(str < "test string!");
    EXPECT_FALSE(str < "test-");
    EXPECT_TRUE(str < "test strinj");
    // <=
    EXPECT_TRUE(str <= str);
    EXPECT_TRUE(str <= "test string!!!");
    EXPECT_FALSE(str <= "...");
    EXPECT_TRUE(str <= "test strinj");
}

TEST_F(string_view, find) {
    EXPECT_EQ(str.find("test"), 0);
    EXPECT_EQ(str.find("string"), 5);
    EXPECT_EQ(str.find("e"), 1);
    EXPECT_EQ(str.find("s", 3), 5);

    EXPECT_EQ(str.find("test string!!!"), str.size());
    EXPECT_EQ(str.find("hello"), str.size());
    EXPECT_EQ(str.find(""), 0);
    EXPECT_EQ(str.find("", 3), 3);
    EXPECT_EQ(str.find("string", 8), str.size());

    EXPECT_EQ(str.find('t'), 0);
    EXPECT_EQ(str.find('g'), 10);
    EXPECT_EQ(str.find('x'), str.size());
    EXPECT_EQ(str.find('g', 11), str.size());
}

}
