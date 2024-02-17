#include <catch2/catch_test_macros.hpp>

#include <array>
#include <betterstring/string.hpp>

namespace {

TEST_CASE("constructor", "[string]") {
    SECTION("default") {
        bs::string str;
        CHECK(str.size() == 0);
        CHECK(str == "");
    }
    SECTION("from pointer and size") {
        bs::string short_str{"test", 4};
        CHECK(short_str.size() == 4);
        CHECK(short_str == "test");

        bs::string long_str{"long string long string long string", 35};
        CHECK(long_str.size() == 35);
        CHECK(long_str == "long string long string long string");
    }
    SECTION("filled") {
        auto short_str = bs::string::filled('a', 5);
        CHECK(short_str.size() == 5);
        CHECK(short_str == "aaaaa");

        auto long_str = bs::string::filled('X', 50);
        CHECK(long_str.size() == 50);
        CHECK(long_str == "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
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
        CHECK(str1 == "test");

        str = "long long long long long long long long string";
        bs::string str2{str};
        CHECK(str2 == "long long long long long long long long string");

        bs::string str3{str3};
        CHECK(str3 == "");
    }
    SECTION("move construct") {
        bs::string str1{bs::string{"test", 4}};
        CHECK(str1 == "test");
        bs::string str2{bs::string{"long long long long long long long string", 41}};
        CHECK(str2 == "long long long long long long long string");
        bs::string str3{std::move(str3)};
        CHECK(str3 == "");
    }
    SECTION("transfer ownership") {
        char* const str_arr1 = new char[5];
        auto str = bs::string::transfer_ownership(str_arr1, 5);
        CHECK(str == "");
        str = "test";
        CHECK(str == "test");

        char* const str_arr2 = new char[50];
        str = bs::string::transfer_ownership(str_arr2, 50);
        CHECK(str == "");
        str = "long long long long long long long";
        CHECK(str == "long long long long long long long");

        char* const str_arr3 = new char[40];
        bs::strcopy(str_arr3, "hello world", 11);
        str = bs::string::transfer_ownership(str_arr3, 11, 40);
        CHECK(str == "hello world");
        str = "123451234512345123451234512345123451";
        CHECK(str == "123451234512345123451234512345123451");
    }
    SECTION("from range") {
        const char arr1[] = "test string";
        bs::string str1{arr1, arr1 + 11};
        CHECK(str1 == "test string");

        std::array arr2 = {'a', 'b', 'c', 'd'};
        bs::string str2{arr2.begin(), arr2.end()};
        CHECK(str2 == "abcd");
    }
    SECTION("from string view like") {
        bs::string str1{bs::string_view{"string"}};
        CHECK(str1 == "string");

        bs::string str2{bs::string_view{"                                        "}};
        CHECK(str2 == "                                        ");
    }
}

TEST_CASE("operator=", "[string]") {
    SECTION("assign from C-string") {
        bs::string str;
        str = "test string";
        CHECK(str == "test string");
        str = "";
        CHECK(str == "");
        str = "long string long string long string long string";
        CHECK(str == "long string long string long string long string");
        str = "long string long string long string long";
        CHECK(str == "long string long string long string long");
        str = "small string";
        CHECK(str == "small string");
    }
    SECTION("assign from string view like") {
        bs::string str;
        str = bs::string_view{"test string"};
        CHECK(str == "test string");
        str = bs::string_view{"test ", 4};
        CHECK(str == "test");
        str = bs::string_view{"long string long string long string ", 35};
        CHECK(str == "long string long string long string");
    }
    SECTION("assign from another string") {
        bs::string str;
        bs::string str1{"test string", 11};
        str = str1;
        CHECK(str == "test string");
        bs::string str2{"long string long string long string long string", 47};
        str = str2;
        CHECK(str == "long string long string long string long string");
        str = str;
        CHECK(str == "long string long string long string long string");
    }
    SECTION("move from another string") {
        bs::string str;
        str = bs::string{"test", 4};
        CHECK(str == "test");
        str = bs::string{"long string long string long string long string", 47};
        CHECK(str == "long string long string long string long string");
        str = bs::string{};
        CHECK(str == "");
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
    CHECK(str == "test string");
    str.clear();
    CHECK(str.size() == 0);
    CHECK(str == "");
}


}
