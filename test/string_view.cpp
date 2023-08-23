#include <gtest/gtest.h>

#include <betterstring.hpp>

namespace {

struct string_view : ::testing::Test {
    bs::string_view<> str{"test string"};
    const bs::string_view<> const_str{"test string"};
};

TEST_F(string_view, constructor) {
    const bs::string_view<> empty_str;
    EXPECT_EQ(empty_str.size(), 0);
    EXPECT_EQ(empty_str.data(), nullptr);

    const bs::string_view<> copy_str(str);
    EXPECT_EQ(copy_str, str);

    const bs::string_view<> cut_str("test string", 4);
    EXPECT_EQ(cut_str, "test");

    const bs::string_view<> cstr_view("test str");
    EXPECT_EQ(cstr_view, "test str");

    constexpr auto cstring = "123456789";
    const bs::string_view<> range_str(cstring + 1, cstring + 7);
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

    EXPECT_EQ(str[1u], 'e');
    EXPECT_EQ(str[2u], 's');

    EXPECT_EQ(str[-1], 'g');
    EXPECT_EQ(str[-11], 't');
}

TEST_F(string_view, front_back) {
    EXPECT_EQ(str.front(), 't');
    EXPECT_EQ(str.back(), 'g');
}

TEST_F(string_view, data) {
    const auto cstring = "hello world";
    const bs::string_view strview(cstring);
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

    EXPECT_EQ(str(0, -5), "test s");
    EXPECT_EQ(str(-5, {}), "tring");

    EXPECT_EQ(str(5u, {}), "string");
    EXPECT_EQ(str(5, -2), "stri");

    EXPECT_EQ(str[bs::slice(2)], "te");
    EXPECT_EQ(str[bs::slice(0, 5)], "test ");
    EXPECT_EQ(str[bs::slice(5, 5)], "");
    EXPECT_EQ(str[bs::slice(5, 11)], "string");
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

    EXPECT_EQ(str.find("str", 0, 4), 4);
    EXPECT_EQ(str.find("string", 5, 11), 5);

    EXPECT_EQ(str.find('t'), 0);
    EXPECT_EQ(str.find('g'), 10);
    EXPECT_EQ(str.find('x'), str.size());
    EXPECT_EQ(str.find('g', 11), str.size());

    EXPECT_EQ(str.find('r', 5, 8), 7);
}

TEST_F(string_view, rfind) {
    EXPECT_EQ(str.rfind(' '), 4);
    EXPECT_EQ(str.rfind('t'), 6);
    EXPECT_EQ(bs::string_view("hello").rfind('h'), 0);
    EXPECT_EQ(str.rfind('g'), 10);
    EXPECT_EQ(str.rfind('t', 3), 0);
    EXPECT_EQ(str.rfind('t', 11, 7), 7);
    EXPECT_EQ(str.rfind(' ', 7, 1), 4);

    EXPECT_EQ(str.rfind("test"), 0);
    EXPECT_EQ(str.rfind("string"), 5);
    EXPECT_EQ(str.rfind(" "), 4);
    EXPECT_EQ(str.rfind("test", 8), 0);
    EXPECT_EQ(str.rfind("string", 10), 0);
    EXPECT_EQ(str.rfind("st", str.size(), 4), 5);
}

TEST_F(string_view, split) {
    unsigned index = 0;
    for (auto sub : str.split(" ")) {
        if (index == 0) EXPECT_EQ(sub, "test");
        if (index == 1) EXPECT_EQ(sub, "string");
        if (index == 2) ADD_FAILURE();
        ++index;
    }
    index = 0; // "test string"
    for (auto sub : str.split("t")) {
        if (index == 0) EXPECT_EQ(sub, "");
        if (index == 1) EXPECT_EQ(sub, "es");
        if (index == 2) EXPECT_EQ(sub, " s");
        if (index == 3) EXPECT_EQ(sub, "ring");
        if (index == 4) ADD_FAILURE();
        ++index;
    }
    index = 0;
    for (auto sub : bs::string_view("aaaa").split(" ")) {
        if (index == 0) EXPECT_EQ(sub, "aaaa");
        if (index == 1) ADD_FAILURE();
        ++index;
    }

    EXPECT_EQ(str.split(" ")[0], "test");
    EXPECT_EQ(str.split(" ")[1], "string");
    EXPECT_EQ(bs::string_view("  ").split(" ")[1], "");
}

TEST_F(string_view, strip) {
    const bs::string_view<> test_strings[]{" hello ", "hello ", " hello", "  hello", "hello  ", "  hello  "};
    for (const auto s : test_strings) {
        EXPECT_EQ(s.strip(' '), "hello");
    }
    EXPECT_EQ(str.strip("test"), " string");
    EXPECT_EQ(str.strip("tes"), " string");
    EXPECT_EQ(bs::string_view(" \thellow\n ").strip(" \t\n"), "hellow");
    EXPECT_EQ(bs::string_view("aaaaa").strip("a"), "");

    EXPECT_EQ(str.strip(), "test string");
}

}
