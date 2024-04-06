
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
#include <betterstring/functions.hpp>
#include <util.hpp>

namespace {

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
        CHECK(bs::strfind(test_str, 11, "test", 4) == &test_str[0]);
        CHECK(bs::strfind(test_str, 4, "test", 4) == &test_str[0]);
        CHECK(bs::strfind(test_str, 4, "t st", 4) == nullptr);
        CHECK(bs::strfind(test_str, 4, "test ", 5) == nullptr);
        CHECK(bs::strfind(test_str, 11, "", 0) == &test_str[0]);
        CHECK(bs::strfind(test_str, 11, "test string", 11) == &test_str[0]);

        const char* const test_string1 = "long1 string1 long2 string2 long3 string3";
        CHECK(bs::strfind(test_string1, 41, "long3 string3", 13) == &test_string1[28]);
        CHECK(bs::strfind(test_string1, 41, "ng2 string", 10) == &test_string1[16]);
        CHECK(bs::strfind(test_string1, 41, " long1", 6) == nullptr);
        CHECK(bs::strfind(test_string1, 41, "long", 4) == &test_string1[0]);
        CHECK(bs::strfind(test_string1, 41, "long1", 5) == &test_string1[0]);
        CHECK(bs::strfind(test_string1, 41, "string", 6) == &test_string1[6]);
        CHECK(bs::strfind(test_string1, 41, "string3", 7) == &test_string1[34]);
        CHECK(bs::strfind(test_string1, 32, "long1 string1 long2 string2 long", 32) == &test_string1[0]);

        const char* const test_string2 = "aaaaaaaaaaaaaaaaaaaaaab baab baaaaaaabcbaaaa";
        CHECK(bs::strfind(test_string2, 44, "bcb", 3) == &test_string2[37]);

        const char* const test_string3 = "0123456789010111213141516171819202122232425262728293031323334353";
        CHECK(bs::strfind(test_string3, 64, "01234", 5) == &test_string3[0]);
        CHECK(bs::strfind(test_string3, 64, "353", 3) == &test_string3[61]);
        CHECK(bs::strfind(test_string3, 64, "2223", 4) == &test_string3[35]);
        CHECK(bs::strfind(test_string3, 64, "02122232425262728293031323334353", 32) == &test_string3[32]);
        CHECK(bs::strfind(test_string3, 64, "021", 3) == &test_string3[32]);

        const char* const test_string4 = "012345678910111213141516171819202122232425262728293031323334353637383940414243444546474849505152";
        CHECK(bs::strfind(test_string4, 96, "3444", 4) == &test_string4[77]);
        CHECK(bs::strfind(test_string4, 96, "37383940414243444546474849505152", 32) == &test_string4[64]);
        CHECK(bs::strfind(test_string4, 96, "373", 3) == &test_string4[64]);

        char* const haystack_page = static_cast<char*>(page_alloc());
        std::memcpy(haystack_page + (4096 - 15), "012345678910111", 15);
        CHECK(bs::strfind(haystack_page + (4096 - 15), 15, "101", 3) == &haystack_page[4096 - 15 + 10]);
        CHECK(bs::strfind(haystack_page + (4096 - 15), 15, "111", 3) == &haystack_page[4096 - 15 + 12]);
        CHECK(bs::strfind(haystack_page + (4096 - 15), 15, "999", 3) == nullptr);
        CHECK(bs::strfind(haystack_page + (4096 - 15), 15, "012345678910111", 15) == &haystack_page[4096 - 15]);
        CHECK(bs::strfind(haystack_page + (4096 - 15), 15, "012345", 6) == &haystack_page[4096 - 15 + 0]);
        CHECK(bs::strfind(haystack_page + (4096 - 15), 15, "1", 1) == &haystack_page[4096 - 15 + 1]);
        CHECK(bs::strfind(haystack_page + (4096 - 15), 15, "91", 2) == &haystack_page[4096 - 15 + 9]);

        std::memcpy(haystack_page + (4096 - 32), "iyigderktirkuwqfyrmeyjgarmfafwer", 32);
        CHECK(bs::strfind(haystack_page + (4096 - 32), 32, "yjg", 3) == &haystack_page[4096 - 32 + 20]);
        CHECK(bs::strfind(haystack_page + (4096 - 32), 32, "iyigderktirkuwqfyrmeyjgarmfafwer", 32) == &haystack_page[4096 - 32 + 0]);
        CHECK(bs::strfind(haystack_page + (4096 - 32), 32, "iyigderkt", 9) == &haystack_page[4096 - 32 + 0]);
        CHECK(bs::strfind(haystack_page + (4096 - 32), 32, "wqfyrmeyj", 9) == &haystack_page[4096 - 32 + 13]);
        CHECK(bs::strfind(haystack_page + (4096 - 32), 32, "o", 1) == nullptr);

        std::memcpy(haystack_page + (4096 - 33), "hamfasdjhsdfykaliiihhkfjkihrfhjau", 33);
        CHECK(bs::strfind(haystack_page + (4096 - 33), 33, "jau", 3) == &haystack_page[4096 - 33 + 30]);
        CHECK(bs::strfind(haystack_page + (4096 - 33), 33, "au", 2) == &haystack_page[4096 - 33 + 31]);
        CHECK(bs::strfind(haystack_page + (4096 - 33), 33, "u", 1) == &haystack_page[4096 - 33 + 32]);

        std::memcpy(haystack_page + (4096 - 40), "sdwlfazhjkgyhtgfhafvgmbfatdrjasewerfyhju", 40);
        CHECK(bs::strfind(haystack_page + (4096 - 40), 40, "sdwlf", 5) == &haystack_page[4096 - 40 + 0]);
        CHECK(bs::strfind(haystack_page + (4096 - 40), 40, "hjkgyhtgfhafvgmbfat", 19) == &haystack_page[4096 - 40 + 7]);
        CHECK(bs::strfind(haystack_page + (4096 - 40), 40, "q", 1) == nullptr);
        CHECK(bs::strfind(haystack_page + (4096 - 40), 40, "asew", 4) == &haystack_page[4096 - 40 + 29]);

        std::memcpy(haystack_page + (4096 - 90), "012345678910111213141516171819202122232425262728293031323334353637383940414243444546474849", 90);
        CHECK(bs::strfind(haystack_page + (4096 - 90), 90, "415161718", 9) == &haystack_page[4096 - 90 + 19]);
        CHECK(bs::strfind(haystack_page + (4096 - 90), 90, "223", 3) == &haystack_page[4096 - 90 + 35]);
        CHECK(bs::strfind(haystack_page + (4096 - 90), 90, "3940", 4) == &haystack_page[4096 - 90 + 68]);
        CHECK(bs::strfind(haystack_page + (4096 - 90), 90, "4142434", 7) == &haystack_page[4096 - 90 + 72]);
        CHECK(bs::strfind(haystack_page + (4096 - 90), 90, "0414243444546474849", 19) == &haystack_page[4096 - 90 + 71]);
        CHECK(bs::strfind(haystack_page + (4096 - 90), 90, "01234567891011121314151617181920", 32) == &haystack_page[4096 - 90 + 0]);
        CHECK(bs::strfind(haystack_page + (4096 - 90), 90, "21222324252627282930313233343536", 32) == &haystack_page[4096 - 90 + 32]);
        CHECK(bs::strfind(haystack_page + (4096 - 90), 90, "37383940414243444546474849", 26) == &haystack_page[4096 - 90 + 64]);

        std::memcpy(haystack_page, "012345678910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364", 120);
        CHECK(bs::strfind(haystack_page, 120, "61626", 5) == &haystack_page[112]);
        CHECK(bs::strfind(haystack_page, 120, "1626364", 7) == &haystack_page[113]);
        CHECK(bs::strfind(haystack_page, 120, "0123456", 7) == &haystack_page[0]);
        CHECK(bs::strfind(haystack_page, 120, "abcdef", 5) == nullptr);

        std::memcpy(haystack_page + (4096 - 115), "0123456789101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354555657585960616", 115);
        CHECK(bs::strfind(haystack_page + (4096 - 115), 115, "7585960616", 10) == &haystack_page[4096 - 115 + 105]);
        CHECK(bs::strfind(haystack_page + (4096 - 115), 115, "6474849", 7) == &haystack_page[4096 - 115 + 83]);
        CHECK(bs::strfind(haystack_page + (4096 - 115), 115, "960616_", 7) == nullptr);
        CHECK(bs::strfind(haystack_page + (4096 - 115), 115, "647484_", 7) == nullptr);

        std::memcpy(haystack_page, "012345678910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879", 150);
        CHECK(bs::strfind(haystack_page, 150, "76777879", 8) == &haystack_page[142]);
        CHECK(bs::strfind(haystack_page, 150, "646", 3) == &haystack_page[118]);
        CHECK(bs::strfind(haystack_page, 150, "2636465", 7) == &haystack_page[115]);
        CHECK(bs::strfind(haystack_page, 150, "60616263", 8) == &haystack_page[110]);
        CHECK(bs::strfind(haystack_page, 150, "48495", 5) == &haystack_page[86]);

        std::memcpy(haystack_page + (4096 - 140), "55565758596061626364656667686970717273747576777879808182838485868788899091929394959697989910010110210310410510610710810911011111211311411511", 140);
        CHECK(bs::strfind(haystack_page + (4096 - 140), 140, "1411511", 7) == &haystack_page[4096 - 140 + 133]);
        CHECK(bs::strfind(haystack_page + (4096 - 140), 140, "10610710", 8) == &haystack_page[4096 - 140 + 108]);
        CHECK(bs::strfind(haystack_page + (4096 - 140), 140, "510610710", 9) == &haystack_page[4096 - 140 + 107]);

        std::memcpy(haystack_page, "012345678910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394959697", 186);
        CHECK(bs::strfind(haystack_page, 186, "4959697", 7) == &haystack_page[179]);
        CHECK(bs::strfind(haystack_page, 186, "82838", 5) == &haystack_page[154]);
        CHECK(bs::strfind(haystack_page, 186, "666768", 6) == &haystack_page[122]);
        CHECK(bs::strfind(haystack_page, 186, "8081", 4) == &haystack_page[150]);
        CHECK(bs::strfind(haystack_page, 186, "86878889909192939495", 20) == &haystack_page[162]);

        std::memcpy(haystack_page, "012345678910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394959697989910010110210310410510610710810911011111211311411511611711811912012112212", 261);
        CHECK(bs::strfind(haystack_page, 261, "172737", 6) == &haystack_page[133]);
        CHECK(bs::strfind(haystack_page, 261, "485868", 6) == &haystack_page[159]);
        CHECK(bs::strfind(haystack_page, 261, "788899", 6) == &haystack_page[165]);
        CHECK(bs::strfind(haystack_page, 261, "001011", 6) == &haystack_page[191]);
        CHECK(bs::strfind(haystack_page, 261, "031041", 6) == &haystack_page[200]);
        CHECK(bs::strfind(haystack_page, 261, "011111", 6) == &haystack_page[222]);
        CHECK(bs::strfind(haystack_page, 261, "141151", 6) == &haystack_page[233]);
        CHECK(bs::strfind(haystack_page, 261, "112212", 6) == &haystack_page[255]);

        std::memcpy(haystack_page, "012345678910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394959697989910010110210310410510610710810911011111211311411511611711811912012112212312412512", 270);
        CHECK(bs::strfind(haystack_page, 270, "412512", 6) == &haystack_page[264]);
        CHECK(bs::strfind(haystack_page, 270, "116117", 6) == &haystack_page[238]);
        CHECK(bs::strfind(haystack_page, 270, "11611711811912012112212312412512", 32) == &haystack_page[238]);
        CHECK(bs::strfind(haystack_page, 270, "13114115", 8) == &haystack_page[230]);
        CHECK(bs::strfind(haystack_page, 270, "05106107", 8) == &haystack_page[206]);
        CHECK(bs::strfind(haystack_page, 270, "92939495969798991", 17) == &haystack_page[174]);

        std::memset(haystack_page, 'a', 4096);
        std::memset(haystack_page + (4096 - 5), 'b', 5);
        CHECK(bs::strfind(haystack_page, 4096, "bbbbb", 5) == &haystack_page[4096 - 5]);
        CHECK(bs::strfind(haystack_page, 4096, "ccccc", 5) == nullptr);
        // bad test
        // CHECK(bs::strfind(haystack_page, 4096, "aaaaa", 5) == &haystack_page[0]);

        std::memcpy(haystack_page + (4096 - 180), "012345678910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394", 180);
        CHECK(bs::strfind(haystack_page + (4096 - 180), 180, "091929394", 9) == &haystack_page[4096 - 180 + 171]);
        CHECK(bs::strfind(haystack_page + (4096 - 180), 180, "79808182838", 11) == &haystack_page[4096 - 180 + 148]);
        CHECK(bs::strfind(haystack_page + (4096 - 180), 180, "5767778", 7) == &haystack_page[4096 - 180 + 141]);

        std::memcpy(haystack_page + (4096 - 261), "012345678910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394959697989910010110210310410510610710810911011111211311411511611711811912012112212", 261);
        CHECK(bs::strfind(haystack_page + (4096 - 261), 261, "172737", 6) == &haystack_page[4096 - 261 + 133]);
        CHECK(bs::strfind(haystack_page + (4096 - 261), 261, "485868", 6) == &haystack_page[4096 - 261 + 159]);
        CHECK(bs::strfind(haystack_page + (4096 - 261), 261, "788899", 6) == &haystack_page[4096 - 261 + 165]);
        CHECK(bs::strfind(haystack_page + (4096 - 261), 261, "001011", 6) == &haystack_page[4096 - 261 + 191]);
        CHECK(bs::strfind(haystack_page + (4096 - 261), 261, "031041", 6) == &haystack_page[4096 - 261 + 200]);
        CHECK(bs::strfind(haystack_page + (4096 - 261), 261, "011111", 6) == &haystack_page[4096 - 261 + 222]);
        CHECK(bs::strfind(haystack_page + (4096 - 261), 261, "141151", 6) == &haystack_page[4096 - 261 + 233]);
        CHECK(bs::strfind(haystack_page + (4096 - 261), 261, "112212", 6) == &haystack_page[4096 - 261 + 255]);

        std::memcpy(haystack_page + (4096 - 270), "012345678910111213141516171819202122232425262728293031323334353637383940414243444546474849505152535455565758596061626364656667686970717273747576777879808182838485868788899091929394959697989910010110210310410510610710810911011111211311411511611711811912012112212312412512", 270);
        CHECK(bs::strfind(haystack_page + (4096 - 270), 270, "412512", 6) == &haystack_page[4096 - 270 + 264]);
        CHECK(bs::strfind(haystack_page + (4096 - 270), 270, "116117", 6) == &haystack_page[4096 - 270 + 238]);
        CHECK(bs::strfind(haystack_page + (4096 - 270), 270, "11611711811912012112212312412512", 32) == &haystack_page[4096 - 270 + 238]);
        CHECK(bs::strfind(haystack_page + (4096 - 270), 270, "13114115", 8) == &haystack_page[4096 - 270 + 230]);
        CHECK(bs::strfind(haystack_page + (4096 - 270), 270, "05106107", 8) == &haystack_page[4096 - 270 + 206]);
        CHECK(bs::strfind(haystack_page + (4096 - 270), 270, "92939495969798991", 17) == &haystack_page[4096 - 270 + 174]);

        char* const needle_page = static_cast<char*>(page_alloc());

        const char* const str1 = "0123456789101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354";
        std::memcpy(needle_page + (4096 - 20), "45464748495051525354", 20);
        CHECK(bs::strfind(str1, 100, needle_page + (4096 - 20), 20) == &str1[80]);
        CHECK(bs::strfind(str1, 100, needle_page + 4096, 0) == &str1[0]);
        std::memcpy(needle_page + (4096 - 32), "72829303132333435363738394041424", 32);
        CHECK(bs::strfind(str1, 100, needle_page + (4096 - 32), 32) == &str1[45]);

        std::memcpy(haystack_page + (4096 - 75), "012345678910111213141516171819202122232425262728293031323334353637383940414", 75);
        std::memcpy(needle_page, "3435363738394", 13);
        CHECK(bs::strfind(haystack_page + (4096 - 75), 75, needle_page, 13) == &haystack_page[4096 - 75 + 58]);
        CHECK(bs::strfind(haystack_page + (4096 - 75), 75, needle_page, 0) == &haystack_page[4096 - 75 + 0]);
        std::memcpy(needle_page, "10111213141516171819202122232425", 32);
        CHECK(bs::strfind(haystack_page + (4096 - 75), 75, needle_page, 32) == &haystack_page[4096 - 75 + 10]);

        std::memcpy(haystack_page + (4096 - 33 - 2), "abaaaaaababbbbbbbbabaabbaaaabbaaa", 33);
        std::memcpy(needle_page + (4096 - 3), "aca", 3);
        CHECK(bs::strfind(haystack_page + (4096 - 33 - 2), 33, needle_page + (4096 - 3), 3) == nullptr);

        std::memcpy(haystack_page + (4096 - 65 - 25), "acccccccccccccccccccccccccccccccccccccccccacacaabaacaacaacacaacab", 65);
        std::memcpy(needle_page + (4096 - 2 - 92), "ab", 2);
        CHECK(bs::strfind(haystack_page + (4096 - 65 - 25), 65, needle_page + (4096 - 2 - 92), 2) == &haystack_page[4096 - 65 - 25 + 47]);

        page_free(haystack_page);

        page_free(needle_page);

        // CHECK(bs::strfind(test_str, 11, "string", 6) == &test_str[5]);
        // CHECK(bs::strfind(test_str, 3, "hello", 5) == nullptr);
        // CHECK(bs::strfind(test_str, 11, nullptr, 0) == test_str);
        // CHECK(bs::strfind(test_str, 11, "test", 4) == &test_str[0]);
        // CHECK(bs::strfind(test_str, 0, "abc", 3) == nullptr);
        // CHECK(bs::strfind(test_str, 11, "str", 3) == &test_str[5]);
        // CHECK(bs::strfind(test_str, 11, "test string", 11) == &test_str[0]);
        // CHECK(bs::strfind(test_str, 11, "test string   ", 14) == nullptr);
        // CHECK(bs::strfind(test_str, 11, "", 0) == &test_str[0]);
        // CHECK(bs::strfind(test_str, 11, " ", 1) == &test_str[4]);
        // 
        // CHECK(bs::strfind(static_cast<char*>(nullptr), 0, "str", 3) == nullptr);
        // CHECK(bs::strfind(static_cast<const char*>(nullptr), 0, "", 0) == nullptr);
    }
}

}
