#include <gtest/gtest.h>
#include <betterstring.hpp>

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

}
