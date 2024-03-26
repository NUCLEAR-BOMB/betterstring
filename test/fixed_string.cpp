#include <betterstring/fixed_string.hpp>

#include <catch2/catch_test_macros.hpp>

namespace {

TEST_CASE("constructors", "[fixed_string]") {
    SECTION("from first and size") {
        bs::fixed_string<10> str{"helloworld", 10};
        CHECK(str == "helloworld");
    }
    SECTION("from first and last") {
        const char* const arr = "test string";
        bs::fixed_string<11> str{arr, arr + 11};
        CHECK(str == "test string");
    }
    SECTION("from string literal") {
        bs::fixed_string<11> str = "test string";
        CHECK(str == "test string");
    }
    SECTION("from std::array") {
        std::array<char, 11> arr{'t', 'e', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g'};
        bs::fixed_string<11> str{arr};
        CHECK(str == "test string");
    }
}
TEST_CASE("operator==", "[fixed_string]") {
    SECTION("two fixed_string") {
        bs::fixed_string<11> str1 = "test string";
        bs::fixed_string<11> str2 = "hello world";
        CHECK_FALSE(str1 == str2);

        bs::fixed_string<11> str3 = "test string";
        bs::fixed_string<11> str4 = "test string";
        CHECK(str3 == str4);
    }
    SECTION("right string literal") {
        bs::fixed_string<11> str1 = "hello world";
        CHECK(str1 == "hello world");
        CHECK(str1 != "helloworld");
    }
    SECTION("left string literal") {
        bs::fixed_string<11> str1 = "test string";
        CHECK("test string" == str1);
        CHECK("teststring" != str1);
    }
}

}


