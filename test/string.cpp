#include <catch2/catch_test_macros.hpp>

#include <array>
#include <betterstring/string.hpp>

using namespace bs::literals;

namespace {

TEST_CASE("constructor", "[string]") {
    SECTION("default") {
        bs::string str;
        CHECK(str.size() == 0);
        CHECK(str == ""_sv);
    }
    SECTION("from pointer and size") {
        bs::string short_str{"test", 4};
        CHECK(short_str.size() == 4);
        CHECK(short_str == "test"_sv);

        bs::string long_str{"long string long string long string", 35};
        CHECK(long_str.size() == 35);
        CHECK(long_str == "long string long string long string"_sv);

        bs::string short_str2{"\0test\0string\0", 13};
        CHECK(short_str2 == "\0test\0string\0"_sv);
    }
    SECTION("filled") {
        auto short_str = bs::string::filled('a', 5);
        CHECK(short_str.size() == 5);
        CHECK(short_str == "aaaaa"_sv);

        auto long_str = bs::string::filled('X', 50);
        CHECK(long_str.size() == 50);
        CHECK(long_str == "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"_sv);

        auto nt_str = bs::string::filled('\0', 10);
        CHECK(nt_str.size() == 10);
        CHECK(nt_str == "\0\0\0\0\0\0\0\0\0\0"_sv);
    }
    SECTION("with capacity") {
        auto short_str = bs::string::with_capacity(10);
        CHECK(short_str.size() == 0);
        CHECK(short_str.capacity() >= 10);

        auto long_str = bs::string::with_capacity(30);
        CHECK(long_str.size() == 0);
        CHECK(long_str.capacity() == 30);
    }

    SECTION("copy construct") {
        bs::string str{"test", 4};
        bs::string str1{str};
        CHECK(str1 == "test"_sv);

        str = "long long long long long long long long string"_sv;
        bs::string str2{str};
        CHECK(str2 == "long long long long long long long long string"_sv);
    }
    SECTION("move construct") {
        bs::string str1{bs::string{"test", 4}};
        CHECK(str1 == "test"_sv);
        bs::string str2{bs::string{"long long long long long long long string", 41}};
        CHECK(str2 == "long long long long long long long string"_sv);
    }
    SECTION("transfer ownership") {
        char* const str_arr1 = new char[5];
        auto str = bs::string::transfer_ownership(str_arr1, 5);
        CHECK(str == ""_sv);
        str = "test"_sv;
        CHECK(str == "test"_sv);

        char* const str_arr2 = new char[50];
        str = bs::string::transfer_ownership(str_arr2, 50);
        CHECK(str == ""_sv);
        str = "long long long long long long long"_sv;
        CHECK(str == "long long long long long long long"_sv);

        char* const str_arr3 = new char[40];
        bs::strcopy(str_arr3, "hello world", 11);
        str = bs::string::transfer_ownership(str_arr3, 11, 40);
        CHECK(str == "hello world"_sv);
        str = "123451234512345123451234512345123451"_sv;
        CHECK(str == "123451234512345123451234512345123451"_sv);
    }
    SECTION("from range") {
        const char arr1[] = "test string";
        bs::string str1{arr1, arr1 + 11};
        CHECK(str1 == "test string"_sv);

        std::array arr2 = {'a', 'b', 'c', 'd'};
        bs::string str2{arr2.begin(), arr2.end()};
        CHECK(str2 == "abcd"_sv);
    }
    SECTION("from string view like") {
        bs::string str1{bs::string_view{"string"}};
        CHECK(str1 == "string"_sv);

        bs::string str2{bs::string_view{"                                        "}};
        CHECK(str2 == "                                        "_sv);
    }
}

TEST_CASE("operator=", "[string]") {
    SECTION("assign from string view like") {
        bs::string str;
        str = bs::string_view{"test string"};
        CHECK(str == "test string"_sv);
        str = bs::string_view{"test ", 4};
        CHECK(str == "test"_sv);
        str = bs::string_view{"long string long string long string ", 35};
        CHECK(str == "long string long string long string"_sv);
    }
    SECTION("assign from another string") {
        bs::string str;
        bs::string str1{"test string", 11};
        str = str1;
        CHECK(str == "test string"_sv);
        bs::string str2{"long string long string long string long string", 47};
        str = str2;
        CHECK(str == "long string long string long string long string"_sv);
        str = str;
        CHECK(str == "long string long string long string long string"_sv);
    }
    SECTION("move from another string") {
        bs::string str;
        str = bs::string{"test", 4};
        CHECK(str == "test"_sv);
        str = bs::string{"long string long string long string long string", 47};
        CHECK(str == "long string long string long string long string"_sv);
        str = bs::string{};
        CHECK(str == ""_sv);
    }
}

TEST_CASE(".reserve", "[string]") {
    const size_t sso_capacity = bs::string{}.capacity();
    bs::string str;
    CHECK(str.capacity() == sso_capacity);
    str.reserve(5);
    CHECK(str.capacity() == sso_capacity);
    str.reserve(sso_capacity);
    CHECK(str.capacity() == sso_capacity);
    str.reserve(sso_capacity + 1);
    CHECK(str.capacity() > sso_capacity + 1);
    str.reserve(100);
    CHECK(str.capacity() >= 100);
}

TEST_CASE(".reserve_exact", "[string]") {
    const size_t sso_capacity = bs::string{}.capacity();
    bs::string str;
    CHECK(str.capacity() == sso_capacity);
    str.reserve_exact(5);
    CHECK(str.capacity() == sso_capacity);
    str.reserve_exact(sso_capacity + 1);
    CHECK(str.capacity() == sso_capacity + 1);
    str.reserve_exact(100);
    CHECK(str.capacity() == 100);
}

TEST_CASE(".clear", "[string]") {
    bs::string str{"test string", 11};
    CHECK(str.size() == 11);
    CHECK(str == "test string"_sv);
    str.clear();
    CHECK(str.size() == 0);
    CHECK(str == ""_sv);
}

TEST_CASE(".push_back", "[string]") {
    bs::string str;
    str.push_back('a');
    CHECK(str == "a"_sv);
    str.push_back('b');
    CHECK(str == "ab"_sv);
    str.push_back('c');
    CHECK(str == "abc"_sv);

    str = "12341234123412341234123412341234"_sv;
    str.push_back('a');
    CHECK(str == "12341234123412341234123412341234a"_sv);
    str.push_back('b');
    CHECK(str == "12341234123412341234123412341234ab"_sv);
}

TEST_CASE(".pop_back", "[string]") {
    bs::string str{"test", 4};
    str.pop_back();
    CHECK(str.back() == 's');
    str.pop_back();
    CHECK(str.back() == 'e');
    str.pop_back();
    CHECK(str.back() == 't');
}

TEST_CASE("operator[]", "[string]") {
    SECTION("signed integer index") {
        bs::string str{"abcd", 4};
        CHECK(str[0] == 'a');
        CHECK(str[1] == 'b');
        CHECK(str[2] == 'c');
        CHECK(str[3] == 'd');

        CHECK(str[-1] == 'd');
        CHECK(str[-2] == 'c');
        CHECK(str[-3] == 'b');
        CHECK(str[-4] == 'a');

        const bs::string const_str = str;
        CHECK(const_str[0] == 'a');
        CHECK(const_str[1] == 'b');
        CHECK(const_str[2] == 'c');
        CHECK(const_str[3] == 'd');

        CHECK(const_str[-1] == 'd');
        CHECK(const_str[-2] == 'c');
        CHECK(const_str[-3] == 'b');
        CHECK(const_str[-4] == 'a');
    }
    SECTION("unsigned integer index") {
        bs::string str{"abcd", 4};
        CHECK(str[0U] == 'a');
        CHECK(str[1U] == 'b');
        CHECK(str[2U] == 'c');
        CHECK(str[3U] == 'd');

        const bs::string const_str = str;
        CHECK(const_str[0U] == 'a');
        CHECK(const_str[1U] == 'b');
        CHECK(const_str[2U] == 'c');
        CHECK(const_str[3U] == 'd');
    }
}

TEST_CASE(".back", "[string]") {
    bs::string str{"1234", 4};
    CHECK(str.back() == '4');
    str = "123"_sv;
    CHECK(str.back() == '3');
    str = "1"_sv;
    CHECK(str.back() == '1');
}

TEST_CASE(".front", "[string]") {
    bs::string str{"1234", 4};
    CHECK(str.front() == '1');
    str = "234"_sv;
    CHECK(str.front() == '2');
    str = "4"_sv;
    CHECK(str.front() == '4');
}

TEST_CASE(".at", "[string]") {
    bs::string str{"test string", 11};
    CHECK(str.at(0) == 't');
    CHECK(str.at(4U) == ' ');
    CHECK(str.at(-1) == 'g');
    CHECK(str.at(-2) == 'n');
    CHECK(str.at(str.size() - 1) == 'g');

    CHECK_FALSE(str.at(3128) == 'a');
    CHECK_FALSE(str.at(-123).has_value());
    CHECK_FALSE(str.at(str.size()) == 'g');
}

TEST_CASE(".at_front", "[string]") {
    bs::string str{"test 123", 8};
    CHECK(str.at_front() == 't');
    str = "123"_sv;
    CHECK(str.at_front() == '1');

    str.clear();
    CHECK_FALSE(str.at_front().has_value());
}

TEST_CASE(".at_back", "[string]") {
    bs::string str{"hello world", 11};
    CHECK(str.at_back() == 'd');
    str = "hello"_sv;
    CHECK(str.at_back() == 'o');

    str.clear();
    CHECK_FALSE(str.at_back().has_value());
}

TEST_CASE(".append", "[string]") {
    SECTION("append fixed-size buffer") {
        bs::string str{"123", 3};
        str.append("test", 4);
        CHECK(str == "123test"_sv);
        str.append("very long string!", 17);
        CHECK(str == "123testvery long string!"_sv);
        str.append("even more big", 13);
        CHECK(str == "123testvery long string!even more big"_sv);
    }
    SECTION("append string view like") {
        bs::string str{"string", 6};
        str.append(bs::string_view{" test 123"});
        CHECK(str == "string test 123"_sv);
    }
    SECTION("append static C-string") {
        bs::string str;
        str.append("hello world");
        CHECK(str == "hello world"_sv);
        str.append("hello!");
        CHECK(str == "hello worldhello!"_sv);
    }
    SECTION("append range") {
        bs::string str;
        const char long_str[] = "12345&12345&12345&12345&12345&12345&12345&12345&12345&12345&";
        str.append(long_str, long_str + 60);
        CHECK(str == "12345&12345&12345&12345&12345&12345&12345&12345&12345&12345&"_sv);
        str.append(long_str, long_str + 1);
        CHECK(str == "12345&12345&12345&12345&12345&12345&12345&12345&12345&12345&1"_sv);
        str.append(long_str, long_str);
        CHECK(str == "12345&12345&12345&12345&12345&12345&12345&12345&12345&12345&1"_sv);
    }
}

TEST_CASE("literals", "[string]") {
    bs::string str = "test string"_s;
    CHECK(str == "test string"_sv);

    CHECK("test"_s == "test"_sv);
    CHECK_FALSE(("hello world"_s) == "hello"_sv);

    auto str2 = "test"_s;
    CHECK(str2 == "test"_s);
}

}
