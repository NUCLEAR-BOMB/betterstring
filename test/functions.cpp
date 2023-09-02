#include <gtest/gtest.h>
#include <betterstring.hpp>
#include <string>
#include "tools.hpp"

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
    
    CONSTEXPR_EXPECT(strlen, {
        if (!(bs::strlen(cstr) == 11)) return 1;
        if (!(bs::strlen(cwstr) == 11)) return 2;
        if (!(bs::strlen(str_view) == 11)) return 3;
        return 0;
    });
}

TEST_F(functions, strcopy) {
    std::string tmp_str(11, ' ');
    bs::strcopy(tmp_str.data(), cstr, tmp_str.size());
    EXPECT_EQ(tmp_str, "test string");

    bs::strcopy(tmp_str.data(), tmp_str.size(), str_view);
    EXPECT_EQ(tmp_str, "test string");

    bs::strcopy(tmp_str.data(), tmp_str.size(), "str");
    EXPECT_EQ(tmp_str, "strt string");

    std::wstring tmp_wstr(11, L' ');
    bs::strcopy(tmp_wstr.data(), cwstr, tmp_str.size());
    EXPECT_EQ(tmp_wstr, L"test string");

    CONSTEXPR_EXPECT(strcopy, {
        char buf[20]{};
        bs::strcopy(buf, 20, "test");
        if (not bs::streq(buf, 4, "test")) return 1;
        bs::strcopy(buf + 5, 15, "string");
        if (not bs::streq(buf, 4 + 1 + 6, "test\0string")) return 2;
        return 0;
    });
}

TEST_F(functions, strcomp) {
    EXPECT_EQ(bs::strcomp(cstr, cstr, bs::strlen(cstr)), 0);
    EXPECT_LT(bs::strcomp("test strind", cstr, 11), 0);

    EXPECT_GT(bs::strcomp(cstr, bs::strlen(cstr), "test strina", 11), 0);
    EXPECT_EQ(bs::strcomp(str_view, str_view), 0);
    EXPECT_GT(bs::strcomp(str_view, str_view(0, -1)), 0);
    EXPECT_LT(bs::strcomp(str_view(1, {}), str_view), 0);

    CONSTEXPR_EXPECT(strcomp, {
        if (!(bs::strcomp(cstr, cstr, bs::strlen(cstr)) == 0)) return 1;
        if (!(bs::strcomp(cstr, bs::strlen(cstr), "test strind") > 0)) return 2;
        if (!(bs::strcomp("test strind", cstr, 11) < 0)) return 3;
        return 0;
    });
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

    EXPECT_EQ(bs::strfind(static_cast<char*>(nullptr), 0, 't'), nullptr);
    EXPECT_EQ(bs::strfind(static_cast<const char*>(nullptr), 0, ' '), nullptr);

    CONSTEXPR_EXPECT(strfind_char, {
        if (!(bs::strfind(str_view, 's') == &str_view[2])) return 1;
        if (!(bs::strfind(cstr, 11, 'g'))) return 2;
        if (!(*bs::strfind("123", '3') == '3')) return 3;
        if (!(bs::strfind(str_view, " ") == &str_view[4])) return 6;
        return 0;
    });
}

TEST_F(functions, strrfind_char) {
    EXPECT_EQ(bs::strrfind(cstr, 5, 'i'), nullptr);
    EXPECT_EQ(bs::strrfind(cstr, 11, 'g'), &cstr[10]);
    EXPECT_EQ(bs::strrfind(str_view, 't'), &str_view[6]);
    EXPECT_EQ(*bs::strrfind("123", '1'), '1');
    EXPECT_EQ(bs::strrfind(cstr, 11, 'x'), nullptr);

    EXPECT_EQ(bs::strrfind(static_cast<char*>(nullptr), 0, 'k'), nullptr);
    EXPECT_EQ(bs::strrfind(static_cast<const char*>(nullptr), 0, 'a'), nullptr);

    CONSTEXPR_EXPECT(strrfind_char, {
        if (!(bs::strrfind(cstr, 11, 'e') == &cstr[1])) return 1;
        if (!(bs::strrfind(cstr, 5, 'g') == nullptr)) return 2;
        if (!(bs::strrfind(str_view, ' ') == &str_view[4])) return 3;
        if (!(bs::strrfind(str_view, 't') == &str_view[6])) return 7;
        return 0;
    });
}

TEST_F(functions, strfind_string) {
    EXPECT_EQ(bs::strfind(cstr, 11, "string"), &cstr[5]);
    EXPECT_EQ(bs::strfind(cstr, 3, "hello"), nullptr);
    EXPECT_EQ(bs::strfind(cstr, 11, nullptr, 0), cstr);
    EXPECT_EQ(bs::strfind(cstr, 11, "test"), &cstr[0]);
    EXPECT_EQ(bs::strfind(cstr, 0, "abc"), nullptr);
    EXPECT_EQ(bs::strfind(str_view, "str"), &str_view[5]);
    EXPECT_EQ(bs::strfind(str_view, "test string"), &str_view[0]);
    EXPECT_EQ(bs::strfind(str_view, "test string   "), nullptr);
    EXPECT_EQ(bs::strfind(str_view, ""), &str_view[0]);
    EXPECT_EQ(bs::strfind(str_view, " "), &str_view[4]);

    EXPECT_EQ(bs::strfind(static_cast<char*>(nullptr), 0, "str"), nullptr);
    EXPECT_EQ(bs::strfind(static_cast<const char*>(nullptr), 0, ""), nullptr);

    CONSTEXPR_EXPECT(strfind_string, {
        if (!(bs::strfind(cstr, 11, "test") == &cstr[0])) return 1;
        if (!(bs::strfind(cstr, 11, "string") == &cstr[5])) return 2;
        if (!(bs::strfind(cstr, 3, "123") == nullptr)) return 3;
        if (!(bs::strfind(str_view, "str") == &str_view[5])) return 4; 
        return 0;
    });
}

TEST_F(functions, strrfind_string) {
    EXPECT_EQ(bs::strrfind(cstr, 11, "t"), &cstr[6]);
    EXPECT_EQ(bs::strrfind(cstr, 11, "test"), &cstr[0]);
    EXPECT_EQ(bs::strrfind(cstr, 3, "helloworld"), nullptr);
    EXPECT_EQ(bs::strrfind(cstr, 11, "string"), &cstr[5]);
    EXPECT_EQ(bs::strrfind(cstr, 0, "123"), nullptr);
    EXPECT_EQ(bs::strrfind(str_view, "st"), &str_view[5]);
    EXPECT_EQ(bs::strrfind(str_view, " "), &str_view[4]);

    EXPECT_EQ(bs::strrfind(static_cast<char*>(nullptr), 0, "123"), nullptr);
    EXPECT_EQ(bs::strrfind(static_cast<const char*>(nullptr), 0, ""), nullptr);

    CONSTEXPR_EXPECT(strrfind_string, {
        if (!(bs::strrfind(cstr, 11, "t") == &cstr[6])) return 1;
        if (!(bs::strrfind(cstr, 11, "test") == &cstr[0])) return 2;
        if (!(bs::strrfind(str_view, "string") == &str_view[5])) return 3;
        if (!(bs::strrfind(str_view, "str") == &str_view[5])) return 4;
        if (!(bs::strrfind(str_view, " ") == &str_view[4])) return 5;
        return 0;
    });
}

TEST_F(functions, cstr) {
    EXPECT_EQ(bs::cstr(cstr), cstr);
    EXPECT_EQ(bs::cstr(str_view), str_view.data());
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

    CONSTEXPR_EXPECT(strfill, {
        char buf[15]{};
        bs::strfill(buf, 14, 'j');
        if (!(bs::streq(buf, 15, "jjjjjjjjjjjjjj\0"))) return 1;
        bs::strfill(buf, 5, ' ');
        if (!(bs::streq(buf, 15, "     jjjjjjjjj\0"))) return 2;
        return 0;
    });
}

TEST_F(functions, strmove) {
    auto* const str1 = new char[10]{};
    bs::strcopy(str1, 10, "hello");
    EXPECT_STREQ(str1, "hello");
    bs::strmove(str1 + 5, str1, 5);
    EXPECT_EQ(bs::strcomp(str1, 10, "hellohello"), 0);
    bs::strmove(str1 + 1, str1, 5);
    EXPECT_EQ(bs::strcomp(str1, 10, "hhelloello"), 0);
    bs::strmove(str1, str1 + 2, 5);
    EXPECT_EQ(bs::strcomp(str1, 10, "elloeoello"), 0);

    delete[] str1;

    CONSTEXPR_EXPECT(strcopy, {
        char buf[10]{};
        bs::strcopy(buf, 10, "test");
        if (!bs::streq(buf, 4, "test")) return 1;
        bs::strmove(buf + 5, buf, 5);
        if (!bs::streq(buf, 9, "test\0test")) return 2;
        bs::strmove(buf + 1, buf, 4);
        if (!bs::streq(buf, 9, "ttesttest")) return 3;
        bs::strmove(buf + 5, buf, 2);
        if (!bs::streq(buf, 9, "ttestttst")) return 4;
        bs::strmove(buf, buf, 9);
        if (!bs::streq(buf, 9, "ttestttst")) return 5;
        return 0;
    });
}

}
