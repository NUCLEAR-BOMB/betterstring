#define _CRT_SECURE_NO_WARNINGS

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

    static_assert(bs::strlen(cstr) == 11);
    static_assert(bs::strlen(cwstr) == 11);
    static_assert(bs::strlen(str_view) == 11);
}

TEST_F(functions, strcpy) {
    std::string tmp_str(11, ' ');
    bs::strcpy(tmp_str.data(), cstr);
    EXPECT_EQ(tmp_str, "test string");

    std::wstring tmp_wstr(11, L' ');
    bs::strcpy(tmp_wstr.data(), cwstr);
    EXPECT_EQ(tmp_wstr, L"test string");
}

}
