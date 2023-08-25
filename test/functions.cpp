#include <gtest/gtest.h>
#include <betterstring.hpp>
#include <string>

namespace {

struct functions : ::testing::Test {
    static constexpr const char* cstr = "test string";
    static constexpr const wchar_t* cwstr = L"test string";
    static constexpr bs::string_view<> str_view = "test string";
};

TEST_F(functions, strlen) {
    EXPECT_EQ(bs::strlen(cstr), 11);
    EXPECT_EQ(bs::strlen(cwstr), 11);
    EXPECT_EQ(bs::strlen(str_view), 11);

    EXPECT_EQ(bs::strlen("hello"), 5);

    static_assert(bs::strlen(cstr) == 11);
    static_assert(bs::strlen(cwstr) == 11);
    static_assert(bs::strlen(str_view) == 11);
}

TEST_F(functions, strcopy) {
    std::string tmp_str(11, ' ');
    bs::strcopy(tmp_str.data(), cstr, tmp_str.size());
    EXPECT_EQ(tmp_str, "test string");

    bs::strcopy(tmp_str.data(), tmp_str.size(), str_view);
    EXPECT_EQ(tmp_str, "test string");

    std::wstring tmp_wstr(11, L' ');
    bs::strcopy(tmp_wstr.data(), cwstr, tmp_str.size());
    EXPECT_EQ(tmp_wstr, L"test string");
}

TEST_F(functions, strcomp) {
    EXPECT_EQ(bs::strcomp(cstr, cstr, bs::strlen(cstr)), 0);
    EXPECT_LT(bs::strcomp("test strind", cstr, 11), 0);

    EXPECT_GT(bs::strcomp(cstr, bs::strlen(cstr), "test strina", 11), 0);
    EXPECT_EQ(bs::strcomp(str_view, str_view), 0);
    EXPECT_GT(bs::strcomp(str_view, str_view(0, -1)), 0);
    EXPECT_LT(bs::strcomp(str_view(1, {}), str_view), 0);
}

TEST_F(functions, empty) {
    EXPECT_TRUE(bs::empty(""));
    EXPECT_FALSE(bs::empty("a"));
    EXPECT_TRUE(bs::empty(bs::string_view("")));
    EXPECT_FALSE(bs::empty(str_view));
    EXPECT_FALSE(bs::empty(cwstr));
}

TEST_F(functions, strfind_char) {
    EXPECT_EQ(bs::strfind(cstr, 11, 't'), cstr);
    EXPECT_EQ(bs::strfind("test", 'g'), nullptr);
    EXPECT_EQ(bs::strfind(str_view, ' '), &str_view[4]);

    static_assert(bs::strfind(str_view, 's') == &str_view[2]);
    static_assert(bs::strfind(cstr, 11, 'g') == &cstr[10]);
}

TEST_F(functions, strrfind) {
    EXPECT_EQ(bs::strrfind(cstr, 5, 'i'), nullptr);
    EXPECT_EQ(bs::strrfind(cstr, 11, 'g'), &cstr[10]);
    EXPECT_EQ(bs::strrfind(str_view, 't'), &str_view[6]);
    EXPECT_EQ(*bs::strrfind("123", '1'), '1');
    EXPECT_EQ(bs::strrfind(cstr, 11, 'x'), nullptr);
}

TEST_F(functions, cstr) {
    EXPECT_EQ(bs::cstr(cstr), cstr);
    EXPECT_EQ(bs::cstr(str_view), str_view.data());
}

TEST_F(functions, strfind_string) {
    EXPECT_EQ(bs::strfind(cstr, 11, "string"), &cstr[5]);
    EXPECT_EQ(bs::strfind(cstr, 3, "hello"), nullptr);
    EXPECT_EQ(bs::strfind(cstr, 11, nullptr, 0), cstr);
    EXPECT_EQ(bs::strfind(cstr, 11, "test"), &cstr[0]);
    EXPECT_EQ(bs::strfind(cstr, 0, "abc"), nullptr);
    EXPECT_EQ(bs::strfind(str_view, "str"), &str_view[5]);
}

TEST_F(functions, strrfind_string) {
    EXPECT_EQ(bs::strrfind(cstr, 11, "t"), &cstr[6]);
    EXPECT_EQ(bs::strrfind(cstr, 11, "test"), &cstr[0]);
    EXPECT_EQ(bs::strrfind(cstr, 3, "helloworld"), nullptr);
    EXPECT_EQ(bs::strrfind(cstr, 11, "string"), &cstr[5]);
    EXPECT_EQ(bs::strrfind(cstr, 0, "123"), nullptr);
    EXPECT_EQ(bs::strrfind(str_view, "st"), &str_view[5]);
    EXPECT_EQ(bs::strrfind(str_view, " "), &str_view[4]);
}

TEST_F(functions, strfill) {
    auto* const new_str = new char[10]{};
    bs::strfill(new_str, 9, 'a');
    EXPECT_STREQ(new_str, "aaaaaaaaa");
    delete[] new_str;

    char stack_str[20]{};
    bs::strfill(stack_str, 10, 'b');
    EXPECT_STREQ(stack_str, "bbbbbbbbbb");

    std::string string(20, 'j');
    bs::strfill(string.data(), string.size(), 'k');
    EXPECT_EQ(string, std::string_view("kkkkkkkkkkkkkkkkkkkk"));
}

}
