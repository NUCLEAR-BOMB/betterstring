
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

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

    bs::strcopy(tmp_str.data(), "test string", 11);
    CHECK(tmp_str == "test string");

    bs::strcopy(tmp_str.data(), "str", 3);
    CHECK(tmp_str == "strt string");

    std::wstring tmp_wstr(11, L' ');
    bs::strcopy(tmp_wstr.data(), L"test string", tmp_str.size());
    CHECK(tmp_wstr == L"test string");
}

TEST_CASE("bs::strcomp", "[functions]") {
    CHECK(bs::strcomp("test string", "test string", 11) == 0);
    CHECK(bs::strcomp("test strind", "test string", 11) < 0);

    CHECK(bs::strcomp("test string", 11, "test strina", 11) > 0);
}

TEST_CASE("bs::strfind", "[functions]") {
    const char* const test_str = "test string";
    SECTION("character") {
        CHECK(bs::strfind(test_str, 11, 't') == test_str);
        CHECK(bs::strfind("test", 4, 'g') == nullptr);
        CHECK(bs::strfind(test_str, 11, ' ') == &test_str[4]);

        CHECK(bs::strfind(static_cast<char*>(nullptr), 0, 't') == nullptr);
        CHECK(bs::strfind(static_cast<const char*>(nullptr), 0, ' ') == nullptr);
    }
    SECTION("string") {
        CHECK(bs::strfind(test_str, 11, "string", 6) == &test_str[5]);
        CHECK(bs::strfind(test_str, 3, "hello", 5) == nullptr);
        CHECK(bs::strfind(test_str, 11, nullptr, 0) == test_str);
        CHECK(bs::strfind(test_str, 11, "test", 4) == &test_str[0]);
        CHECK(bs::strfind(test_str, 0, "abc", 3) == nullptr);
        CHECK(bs::strfind(test_str, 11, "str", 3) == &test_str[5]);
        CHECK(bs::strfind(test_str, 11, "test string", 11) == &test_str[0]);
        CHECK(bs::strfind(test_str, 11, "test string   ", 14) == nullptr);
        CHECK(bs::strfind(test_str, 11, "", 0) == &test_str[0]);
        CHECK(bs::strfind(test_str, 11, " ", 1) == &test_str[4]);

        CHECK(bs::strfind(static_cast<char*>(nullptr), 0, "str", 3) == nullptr);
        CHECK(bs::strfind(static_cast<const char*>(nullptr), 0, "", 0) == nullptr);
    }
}

TEST_CASE("bs::strrfind", "[functions]") {
    const char* const test_str = "test string";

    SECTION("character") {
        CHECK(bs::strrfind("test string", 5, 'i') == nullptr);
        CHECK(bs::strrfind(test_str, 11, 'g') == &test_str[10]);
        CHECK(bs::strrfind(test_str, 11, 't') == &test_str[6]);
        CHECK(*bs::strrfind("123", 3, '1') == '1');
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
        CHECK(bs::strrfind(test_str, 11, "t", 1) == &test_str[6]);
        CHECK(bs::strrfind(test_str, 11, "test", 4) == &test_str[0]);
        CHECK(bs::strrfind(test_str, 3, "helloworld", 10) == nullptr);
        CHECK(bs::strrfind(test_str, 11, "string", 6) == &test_str[5]);
        CHECK(bs::strrfind(test_str, 0, "123", 3) == nullptr);
        CHECK(bs::strrfind(test_str, 11, "st", 2) == &test_str[5]);
        CHECK(bs::strrfind(test_str, 11, " ", 1) == &test_str[4]);

        const auto long_string = "!longlonglonglonglonglonglonglong string";
        CHECK(bs::strrfind(long_string, bs::strlen(long_string), "!long", 5) == &long_string[0]);
        CHECK(bs::strrfind(long_string + 3, bs::strlen(long_string) - 3, "!long", 5) == nullptr);

        CHECK(bs::strrfind(static_cast<char*>(nullptr), 0, "123", 3) == nullptr);
        CHECK(bs::strrfind(static_cast<const char*>(nullptr), 0, "", 0) == nullptr);
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
    CHECK(bs::strcomp(new_str, 9, "aaaaaaaaa", 9) == 0);
    delete[] new_str;

    char stack_str[20]{};
    bs::strfill(stack_str, 10, 'b');
    CHECK(bs::strcomp(stack_str, 10, "bbbbbbbbbb", 10) == 0);

    std::string string(20, 'j');
    bs::strfill(string.data(), string.size(), 'k');
    CHECK(string == std::string_view("kkkkkkkkkkkkkkkkkkkk"));
}

TEST_CASE("bs::strmove", "[functions]") {
    char* const str1 = new char[10]{};
    bs::strcopy(str1, "hello", 5);
    CHECK(bs::strcomp(str1, bs::strlen(str1), "hello", 5) == 0);
    bs::strmove(str1 + 5, str1, 5);
    CHECK(bs::strcomp(str1, "hellohello", 10) == 0);
    bs::strmove(str1 + 1, str1, 5);
    CHECK(bs::strcomp(str1, "hhelloello", 10) == 0);
    bs::strmove(str1, str1 + 2, 5);
    CHECK(bs::strcomp(str1, "elloeoello", 10) == 0);
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
    SECTION("string") {
        CHECK(bs::strcount("hello world", 11, "ll", 2) == 1);
        CHECK(bs::strcount("tteesstt  ssttrriinngg", 22, "st", 2) == 2);
        CHECK(bs::strcount("tteesstt  ssttrriinngg", 22, "tt", 2) == 3);
        CHECK(bs::strcount("tteesstt  ssttrriinngg", 22, " ", 1) == 2);
        CHECK(bs::strcount("tteesstt  ssttrriinngg", 22, "gg", 2) == 1);
        CHECK(bs::strcount("tteesstt  ssttrriinngg", 22, "t", 1) == 6);
        CHECK(bs::strcount("test string", 11, "test string", 11) == 1);
        CHECK(bs::strcount("test string", 11, "test string!", 12) == 0);
        CHECK(bs::strcount("", 0, "", 0) == 1);
        CHECK(bs::strcount("aaaaaaaaaaaaaaaaa", 17, "", 0) == 18);
        CHECK(bs::strcount("", 0, "a", 1) == 0);
        CHECK(bs::strcount("a", 1, "a", 1) == 1);
    }
}

TEST_CASE("bs::strfindn", "[functions]") {
    SECTION("character") {
        const char* str1 = "aaaaaBaaa";
        CHECK(bs::strfindn(str1, 9, 'a') == &str1[5]);
        const char* str2 = "00000000010000";
        CHECK(bs::strfindn(str2, 9, '0') == nullptr);
        CHECK(bs::strfindn(str2, 10, '0') == &str2[9]);

        const char* str3 = "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeE";
        CHECK(bs::strfindn(str3, 32, 'e') == &str3[31]);
        CHECK(bs::strfindn(str3, 31, 'e') == nullptr);
        const char* str4 = "Eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee";
        CHECK(bs::strfindn(str4, 32, 'e') == &str4[0]);
        const char* str5 = "eeeeeeeeeeeeeeeEeeeeeeeeeeeeeeee";
        CHECK(bs::strfindn(str5, 32, 'e') == &str5[15]);

        const char* str6 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab";
        CHECK(bs::strfindn(str6, 33, 'a') == &str6[32]);
        CHECK(bs::strfindn(str6, 32, 'a') == nullptr);
        const char* str7 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaa";
        CHECK(bs::strfindn(str7, 46, 'a') == &str7[38]);
        CHECK(bs::strfindn(str7, 33, 'a') == nullptr);

        char str8[70+1]{};
        bs::strfill(str8, 70, 'a');
        CHECK(bs::strfindn(str8, 70, 'a') == nullptr);
        str8[69] = 'b';
        CHECK(bs::strfindn(str8, 70, 'a') == &str8[69]);
        CHECK(bs::strfindn(str8, 64, 'a') == nullptr);
        str8[63] = 'b';
        CHECK(bs::strfindn(str8, 64, 'a') == &str8[63]);
        str8[33] = 'b';
        CHECK(bs::strfindn(str8, 64, 'a') == &str8[33]);
        CHECK(bs::strfindn(str8, 32, 'a') == nullptr);

        alignas(32) char str9[160+1]{};
        bs::strfill(str9, 160, '0');
        CHECK(bs::strfindn(str9, 160, '0') == nullptr);
        CHECK(bs::strfindn(str9, 65, '0') == nullptr);
        str9[159] = '2';
        CHECK(bs::strfindn(str9, 160, '0') == &str9[159]);
        CHECK(bs::strfindn(str9, 159, '0') == nullptr);
        str9[127] = '1';
        CHECK(bs::strfindn(str9, 160, '0') == &str9[127]);
        CHECK(bs::strfindn(str9, 126, '0') == nullptr);

        alignas(32) char str10[287+1]{};
        bs::strfill(str10, 287, '.');
        CHECK(bs::strfindn(str10, 287, '.') == nullptr);
        CHECK(bs::strfindn(str10, 255, '.') == nullptr);
        str10[286] = ':';
        CHECK(bs::strfindn(str10, 287, '.') == &str10[286]);
        CHECK(bs::strfindn(str10, 285, '.') == nullptr);
        str10[200] = ',';
        CHECK(bs::strfindn(str10, 287, '.') == &str10[200]);
        CHECK(bs::strfindn(str10, 201, '.') == &str10[200]);
        CHECK(bs::strfindn(str10, 200, '.') == nullptr);

        char* const str11 = (char*)page_alloc();
        bs::strfill(str11, 5, 'a');
        CHECK(bs::strfindn(str11, 5, 'a') == nullptr);
        str11[4] = 'b';
        CHECK(bs::strfindn(str11, 5, 'a') == &str11[4]);

        bs::strfill(str11 + (4096 - 32), 32, 'A');
        CHECK(bs::strfindn(str11 + (4096 - 32), 32, 'A') == nullptr);
        str11[4096 - 1] = 'B';
        CHECK(bs::strfindn(str11 + (4096 - 32), 32, 'A') == &str11[4096 - 1]);
        str11[4096 - 32] = 'B';
        CHECK(bs::strfindn(str11 + (4096 - 32), 32, 'A') == &str11[4096 - 32]);

        bs::strfill(str11 + (4096 - 10), 10, 'X');
        CHECK(bs::strfindn(str11 + (4096 - 10), 10, 'X') == nullptr);
        str11[4096 - 1] = 'Y';
        CHECK(bs::strfindn(str11 + (4096 - 10), 10, 'X') == &str11[4096 - 1]);
        str11[4096 - 10] = 'Y';
        CHECK(bs::strfindn(str11 + (4096 - 10), 10, 'X') == &str11[4096 - 10]);

        CHECK(bs::strfindn(str11 + (4096 - 5), 0, 'X') == nullptr);
        CHECK(bs::strfindn(str11 + 4096, 0, 'X') == nullptr);

        page_free(str11);
    }
}

TEST_CASE("bs::strfirstof", "[functions]") {
    CHECK(bs::strfirstof("test", 4, nullptr, 0) == nullptr);

    const char* str1 = "012345";
    CHECK(bs::strfirstof(str1, 6, "4", 1) == &str1[4]);
    CHECK(bs::strfirstof(str1, 6, "6", 1) == nullptr);
    CHECK(bs::strfirstof(str1, 6, "0", 1) == &str1[0]);

    const char* str2 = "abcdefghijklmnopqrstuvwxyz";
    CHECK(bs::strfirstof(str2, 26, "ep", 2) == &str2[4]);
    CHECK(bs::strfirstof(str2, 26, "qu", 2) == &str2[16]);
    CHECK(bs::strfirstof(str2, 26, "zc", 2) == &str2[2]);
    CHECK(bs::strfirstof(str2, 26, "op", 2) == &str2[14]);
    CHECK(bs::strfirstof(str2, 26, "ba", 2) == &str2[0]);

    const char* str3 = "4io8764432kdasjmkde3148158321853271h45iltwergd5q3vbi5vg5rf4jmdg312";
    CHECK(bs::strfirstof(str3, 65, "ABC", 3) == nullptr);

    // const char* str3 = "abcdabcdabcdabcdabcdabcdabcdabcdabcd0";
    // CHECK(bs::strfirst_of(str3, 37, "1234567890", 10) == &str3[36]);
}

TEST_CASE("bs::strfirstnof", "[functions]") {
    const char* str1 = "hello world";
    CHECK(bs::strfirstnof(str1, 11, "hello", 5) == &str1[5]);
    CHECK(bs::strfirstnof(str1, 11, " ", 1) == &str1[0]);
    CHECK(bs::strfirstnof(str1, 11, "helo wrd", 8) == nullptr);
    CHECK(bs::strfirstnof(str1, 11, "helo ", 5) == &str1[6]);
    CHECK(bs::strfirstnof(str1, 11, "", 0) == &str1[0]);
}

TEST_CASE("bs::strlastof", "[functions]") {
    const char* str1 = "test string";
    CHECK(bs::strlastof(str1, 11, "str", 3) == &str1[7]);
    CHECK(bs::strlastof(str1, 11, "t", 1) == &str1[6]);
    CHECK(bs::strlastof(str1, 11, "tttteetsss", 10) == &str1[6]);
    CHECK(bs::strlastof(str1, 11, "test string", 11) == &str1[10]);
    CHECK(bs::strlastof(str1, 11, "test stringtest stringtest string", 33) == &str1[10]);
}

TEST_CASE("bs::strlastnof", "[functions]") {
    const char* str1 = "test string";
    CHECK(bs::strlastnof(str1, 11, "", 0) == &str1[10]);
    CHECK(bs::strlastnof(str1, 11, "test string", 11) == nullptr);
    CHECK(bs::strlastnof(str1, 11, "string", 6) == &str1[4]);
    CHECK(bs::strlastnof(str1, 11, "string ", 7) == &str1[1]);
    CHECK(bs::strlastnof(str1, 11, "g", 1) == &str1[9]);
    CHECK(bs::strlastnof(str1, 11, "ggggggnnnnggiii", 15) == &str1[7]);
}

TEST_CASE("bs::strrfindn", "[functions]") {
    const char* str1 = "hello world";
    CHECK(bs::strrfindn(str1, 11, 'd') == &str1[9]);
    CHECK(bs::strrfindn(str1, 11, 'l') == &str1[10]);
    const char* str2 = "aaaaaaabbbbb";
    CHECK(bs::strrfindn(str2, 12, 'b') == &str2[6]);
    const char* str3 = "bbbbbbbbbbbbbb";
    CHECK(bs::strrfindn(str3, 14, 'b') == nullptr);
}

TEST_CASE("bs::strcountanyof", "[functions]") {
    const char* str1 = "test string";
    CHECK(bs::strcountanyof(str1, 11, " ", 1) == 1);
    CHECK(bs::strcountanyof(str1, 11, "st", 2) == 5);
    CHECK(bs::strcountanyof(str1, 11, "si", 2) == 3);
    CHECK(bs::strcountanyof(str1, 11, "", 0) == 0);
    CHECK(bs::strcountanyof(str1, 11, "ing", 3) == 3);
    CHECK(bs::strcountanyof(str1, 11, "xyz", 3) == 0);
    CHECK(bs::strcountanyof(str1, 11, "txyz", 4) == 3);
}

}
