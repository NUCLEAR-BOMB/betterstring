#define BS_DONT_INCLUDE_STRING

#include <betterstring/string_view.hpp>
#include <betterstring/functions.hpp>
#include <string>
#include <cstdlib>

#include <catch2/catch_test_macros.hpp>

#include "util.hpp"

using namespace bs::literals;

namespace {

TEST_CASE("bs::strlen", "[functions]") {
    CHECK(bs::strlen("test string") == 11);
    CHECK(bs::strlen(L"test string") == 11);

    CHECK(bs::strlen("test\0string") == 4);
    CHECK(bs::strlen("01234567890123456789012345678901") == 32);
    CHECK(bs::strlen("012345678901234567890123456789012345678901234567890123456789") == 60);

    char str1[32*8+1];
    bs::strfill(str1, 32*8, 'A');
    str1[32*8] = '\0';
    CHECK(bs::strlen(str1) == 32 * 8);

    char str2[32*10+1];
    bs::strfill(str2, 32*10, 'A');
    str2[32*10] = '\0';
    CHECK(bs::strlen(str2) == 32*10);

    char* const str3 = (char*)page_alloc();
    bs::strfill(str3, 14, 'B');
    str3[14] = '\0';
    CHECK(bs::strlen(str3) == 14);

    bs::strfill(str3 + (4096 - 10), 9, 'C');
    str3[4096 - 1] = '\0';
    CHECK(bs::strlen(str3 + (4096 - 10)) == 9);
    page_free(str3);
}

TEST_CASE("bs::strcopy", "[functions]") {
    std::string tmp_str(11, ' ');
    bs::strcopy(tmp_str.data(), "test string", tmp_str.size());
    CHECK(tmp_str == "test string");

    bs::strcopy(tmp_str.data(), tmp_str.size(), "test string"_sv);
    CHECK(tmp_str == "test string");

    bs::strcopy(tmp_str.data(), tmp_str.size(), "str");
    CHECK(tmp_str == "strt string");

    std::wstring tmp_wstr(11, L' ');
    bs::strcopy(tmp_wstr.data(), L"test string", tmp_str.size());
    CHECK(tmp_wstr == L"test string");
}

TEST_CASE("bs::strcomp", "[functions]") {
    CHECK(bs::strcomp("test string", "test string", 11) == 0);
    CHECK(bs::strcomp("test strind", "test string", 11) < 0);

    CHECK(bs::strcomp("test string", 11, "test strina", 11) > 0);
    CHECK(bs::strcomp("test string"_sv, "test string"_sv) == 0);
    CHECK(bs::strcomp("test string"_sv, "test string"_sv(0, -1)) > 0);
    CHECK(bs::strcomp("test string"_sv(1, {}), "test string"_sv) < 0);
}

TEST_CASE("bs::strfind", "[functions]") {
    const char* const test_str = "test string";
    const bs::string_view test_strview = "test string"_sv;
    SECTION("character") {
        CHECK(bs::strfind(test_str, 11, 't') == test_str);
        CHECK(bs::strfind("test", 'g') == nullptr);
        CHECK(bs::strfind(test_strview, ' ') == &test_strview[4]);

        CHECK(bs::strfind(static_cast<char*>(nullptr), 0, 't') == nullptr);
        CHECK(bs::strfind(static_cast<const char*>(nullptr), 0, ' ') == nullptr);
    }
    SECTION("string") {
        CHECK(bs::strfind(test_str, 11, "string") == &test_str[5]);
        CHECK(bs::strfind(test_str, 3, "hello") == nullptr);
        CHECK(bs::strfind(test_str, 11, nullptr, 0) == test_str);
        CHECK(bs::strfind(test_str, 11, "test") == &test_str[0]);
        CHECK(bs::strfind(test_str, 0, "abc") == nullptr);
        CHECK(bs::strfind(test_strview, "str") == &test_strview[5]);
        CHECK(bs::strfind(test_strview, "test string") == &test_strview[0]);
        CHECK(bs::strfind(test_strview, "test string   ") == nullptr);
        CHECK(bs::strfind(test_strview, "") == &test_strview[0]);
        CHECK(bs::strfind(test_strview, " ") == &test_strview[4]);

        CHECK(bs::strfind(static_cast<char*>(nullptr), 0, "str") == nullptr);
        CHECK(bs::strfind(static_cast<const char*>(nullptr), 0, "") == nullptr);
    }
}

TEST_CASE("bs::strrfind", "[functions]") {
    const char* const test_str = "test string";
    const bs::string_view test_strview = "test string"_sv;

    SECTION("character") {
        CHECK(bs::strrfind("test string", 5, 'i') == nullptr);
        CHECK(bs::strrfind(test_str, 11, 'g') == &test_str[10]);
        CHECK(bs::strrfind(test_strview, 't') == &test_strview[6]);
        CHECK(*bs::strrfind("123", '1') == '1');
        CHECK(bs::strrfind("test string", 11, 'x') == nullptr);

        const char* const str1 = "abcXabcabcabcabcabcabcabcabcabcabcabcabcabcabcabc";
        CHECK(bs::strrfind(str1, 50, 'X') == &str1[3]);
        CHECK(bs::strrfind(str1, 50, 'Y') == nullptr);
        const char* const str2 = "XbcXabcabcXbcabcabcabcabcabcabcabcabcabcabcabcabc";
        CHECK(bs::strrfind(str2, 50, 'X') == &str2[10]);
        CHECK(bs::strrfind(str2, 50, 'Y') == nullptr);
        const char* const str3 = "abcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcX";
        CHECK(bs::strrfind(str3, 50, 'X') == &str3[48]);
        CHECK(bs::strrfind(str3, 50, 'Y') == nullptr);
        const char* const str4 = "abcabcabcabcabcabcabcabcabcabcabcabcabcabcXabcabc";
        CHECK(bs::strrfind(str4, 50, 'X') == &str4[42]);
        CHECK(bs::strrfind(str4, 50, 'Y') == nullptr);
        const char* const str5 = "abcabcXabcabcabcabcabcabcXabcabcabcabcabcabcabcab";
        CHECK(bs::strrfind(str5, 50, 'X') == &str5[25]);
        CHECK(bs::strrfind(str5, 50, 'Y') == nullptr);

        CHECK(bs::strrfind(static_cast<char*>(nullptr), 0, 'k') == nullptr);
        CHECK(bs::strrfind(static_cast<const char*>(nullptr), 0, 'a') == nullptr);

        CHECK(*bs::strrfind("a", 1, 'a') == 'a');
        CHECK(bs::strrfind("a", 1, 'b') == nullptr);
        CHECK(bs::strrfind("", 0, 'a') == nullptr);
        const char* const str6 = "abcdsldhwamdsbdjfhwaeradjsdgawd";
        CHECK(bs::strrfind(str6, 32, 'X') == nullptr);
        CHECK(bs::strrfind(str6, 32, 'l') == &str6[5]);
        const char* const str7 = "ubcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabc";
        CHECK(bs::strrfind(str7, 64, 'u') == &str7[0]);
        CHECK(bs::strrfind(str7, 64, 'q') == nullptr);
        const char* const str8 = "ubcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd";
        CHECK(bs::strrfind(str8, 45, 'u') == &str8[0]);
        CHECK(bs::strrfind(str8, 45, 'k') == nullptr);

        char str9[32*3+1];
        str9[32*3] = '\0';
        bs::strfill(str9, 32*3, 'y');
        str9[5] = 'b';
        CHECK(bs::strrfind(str9, 32*3, 'a') == nullptr);
        CHECK(bs::strrfind(str9, 32*3, 'b') == &str9[5]);
        CHECK(bs::strrfind(str9, 32*3, 'y') == &str9[32*3 - 1]);

        char str10[32*5+1];
        str10[32*5] = '\0';
        bs::strfill(str10, 32*5, 'a');
        str10[10] = 'q';
        CHECK(bs::strrfind(str10, 32*5, 'h') == nullptr);
        CHECK(bs::strrfind(str10, 32*5, 'q') == &str10[10]);
        CHECK(bs::strrfind(str10, 32*5, 'a') == &str10[32*5-1]);

        char str11[32*8+1];
        str11[32*8] = '\0';
        bs::strfill(str11, 32*8, 'd');
        str11[3] = '&';
        CHECK(bs::strrfind(str11, 32*8, 'i') == nullptr);
        CHECK(bs::strrfind(str11, 32*8, '&') == &str11[3]);
        CHECK(bs::strrfind(str11, 32*8, 'd') == &str11[32*8-1]);

        char* const str12 = (char*)page_alloc();
        bs::strfill(str12 + (4096 - 15), 15, 'q');
        str12[4096 - 15] = 'b';
        CHECK(bs::strrfind(str12 + (4096 - 15), 15, 'a') == nullptr);
        CHECK(bs::strrfind(str12 + (4096 - 15), 15, 'b') == (str12 + (4096 - 15)));
        bs::strfill(str12, 13, 'A');
        CHECK(bs::strrfind(str12, 13, 'B') == nullptr);
        str12[5] = 'B';
        CHECK(bs::strrfind(str12, 13, 'B') == (str12 + 5));
        page_free(str12);
    }
    SECTION("string") {
        CHECK(bs::strrfind(test_str, 11, "t") == &test_str[6]);
        CHECK(bs::strrfind(test_str, 11, "test") == &test_str[0]);
        CHECK(bs::strrfind(test_str, 3, "helloworld") == nullptr);
        CHECK(bs::strrfind(test_str, 11, "string") == &test_str[5]);
        CHECK(bs::strrfind(test_str, 0, "123") == nullptr);
        CHECK(bs::strrfind(test_strview, "st") == &test_strview[5]);
        CHECK(bs::strrfind(test_strview, " ") == &test_strview[4]);

        const auto long_string = "!longlonglonglonglonglonglonglong string";
        CHECK(bs::strrfind(long_string, bs::strlen(long_string), "!long") == &long_string[0]);
        CHECK(bs::strrfind(long_string + 3, bs::strlen(long_string) - 3, "!long") == nullptr);

        CHECK(bs::strrfind(static_cast<char*>(nullptr), 0, "123") == nullptr);
        CHECK(bs::strrfind(static_cast<const char*>(nullptr), 0, "") == nullptr);
    }
}

TEST_CASE("bs::cstr", "[functions]") {
    const char* const test_str = "test";
    CHECK(bs::cstr(test_str) == test_str);

    const std::string test_str2 = "test2";
    CHECK(bs::cstr(test_str2) == &test_str2[0]);

    decltype(auto) a = "array";
    CHECK(bs::cstr(a) == &a[0]);
}

TEST_CASE("bs::strfill", "[functions]") {
    auto* const new_str = new char[10]{};
    bs::strfill(new_str, 9, 'a');
    CHECK(bs::strcomp(new_str, 9, "aaaaaaaaa") == 0);
    delete[] new_str;

    char stack_str[20]{};
    bs::strfill(stack_str, 10, 'b');
    CHECK(bs::strcomp(stack_str, 10, "bbbbbbbbbb") == 0);

    std::string string(20, 'j');
    bs::strfill(string.data(), string.size(), 'k');
    CHECK(string == std::string_view("kkkkkkkkkkkkkkkkkkkk"));
}

TEST_CASE("bs::strmove", "[functions]") {
    char* const str1 = new char[10]{};
    bs::strcopy(str1, 10, "hello");
    CHECK(bs::strcomp(str1, bs::strlen(str1), "hello") == 0);
    bs::strmove(str1 + 5, str1, 5);
    CHECK(bs::strcomp(str1, 10, "hellohello") == 0);
    bs::strmove(str1 + 1, str1, 5);
    CHECK(bs::strcomp(str1, 10, "hhelloello") == 0);
    bs::strmove(str1, str1 + 2, 5);
    CHECK(bs::strcomp(str1, 10, "elloeoello") == 0);
    delete[] str1;
}

TEST_CASE("bs::strcount", "[functions]") {
    SECTION("character") {
        CHECK(bs::strcount("test string", 11, 't') == 3);
        CHECK(bs::strcount(static_cast<char*>(nullptr), 0, 't') == 0);
        CHECK(bs::strcount("", 0, 't') == 0);
        CHECK(bs::strcount("a", 1, 'a') == 1);
        CHECK(bs::strcount("b", 1, 'a') == 0);
        CHECK(bs::strcount("   ", 3, ' ') == 3);
        CHECK(bs::strcount("hello world", 11, 'q') == 0);
        CHECK(bs::strcount("test", 4, 'e') == 1);
        CHECK(bs::strcount("123", 3, '2') == 1);

        CHECK(bs::strcount("1234567890123456789012345678901234567890", 40, '0') == 4);

        char str1[32*5 + 1];
        str1[32*5] = '\0';
        bs::strfill(str1, 32*5, 'a');
        str1[4] = 'b';
        str1[7] = 'b';
        CHECK(bs::strcount(str1, 32*5, 'b') == 2);

        char str2[32*9 + 1];
        str2[32*9] = '\0';
        bs::strfill(str2, 32*9, 'X');
        str2[6] = 'Y';
        str2[23] = 'Y';
        str2[32] = 'Y';
        str2[200] = 'Y';
        CHECK(bs::strcount(str2, 32*9, 'Y') == 4);

        char* const str3 = (char*)page_alloc();
        bs::strfill(str3 + (4096 - 10), 10, 'a');
        CHECK(bs::strcount(str3 + (4096 - 10), 10, 'a') == 10);
        bs::strfill(str3, 11, 'a');
        CHECK(bs::strcount(str3, 11, 'a') == 11);
        page_free(str3);
    }
}

}
