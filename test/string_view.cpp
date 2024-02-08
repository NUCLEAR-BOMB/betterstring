#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <array>

#include <betterstring/string_view.hpp>
#include <betterstring/ascii.hpp>

using namespace bs::literals;

namespace {

TEST_CASE("constructor", "[string_view]") {
    const bs::string_view<> empty_str;
    CHECK(empty_str.size() == 0);
    CHECK(empty_str.data() == nullptr);

    const bs::string_view<> copy_str("test string"_sv);
    CHECK(copy_str == "test string"_sv);

    const bs::string_view<> cut_str("test string", 4);
    CHECK(cut_str == "test");

    const bs::string_view<> cstr_view("test str");
    CHECK(cstr_view == "test str");

    constexpr auto cstring = "123456789";
    const bs::string_view<> range_str(cstring + 1, cstring + 7);
    CHECK(range_str == "234567");
}

TEST_CASE("begin, end", "[string_view]") {
    const bs::string_view str = "test string";
    (void)str.begin();
    (void)str.cbegin();
    (void)str.crbegin();

    (void)str.end();
    (void)str.cend();
    (void)str.crend();
}

TEST_CASE("subscript", "[string_view]") {
    const bs::string_view str = "test string";

    CHECK(str[0] == 't');
    CHECK(str[10] == 'g');

    CHECK(str[0u] == 't');
    CHECK(str[1u] == 'e');
    CHECK(str[2u] == 's');
    CHECK(str[10u] == 'g');

    CHECK(str[-1] == 'g');
    CHECK(str[-11] == 't');
}

TEST_CASE("front, back", "[string_view]") {
    const bs::string_view str = "test string";

    CHECK(str.front() == 't');
    CHECK(str.back() == 'g');
}

TEST_CASE("data", "[string_view]") {
    const char* const cstring = "hello world";
    const bs::string_view strview(cstring);
    CHECK(strview.data() == cstring);
}

TEST_CASE("dataend", "[string_view]") {
    const char* const cstr = "hello";
    const bs::string_view view(cstr);
    CHECK(view.dataend() == cstr + 5);
}

TEST_CASE("size, lenght", "[string_view]") {
    const bs::string_view str = "test string";
    CHECK(str.size() == 11);
    CHECK(str.length() == 11);
}

TEST_CASE("max_size", "[string_view]") {
    CHECK("test string"_sv.max_size() == static_cast<std::size_t>(-1));
}

TEST_CASE("empty", "[string_view]") {
    CHECK("test"_sv.empty() == false);
    CHECK(""_sv.empty() == true);
}

TEST_CASE("remove_prefix", "[string_view]") {
    bs::string_view str = "test string";
    str.remove_prefix(4);
    CHECK(str == " string");
}

TEST_CASE("remove_suffix", "[string_view]") {
    bs::string_view str = "test string";
    str.remove_suffix(3);
    CHECK(str == "test str");
}

TEST_CASE("slice", "[string_view]") {
    const bs::string_view str = "test string";
    CHECK(str(0, 4) == "test");
    CHECK(str(0, 4).size() == 4);
    
    CHECK(str(5, 11) == "string");
    CHECK(str(5u, 10u) == "strin");
    
    CHECK(str(0, -5) == "test s");
    CHECK(str(5, -2) == "stri");
    CHECK(str(7, -1) == "rin");
    CHECK(str(7u, -1) == "rin");

    CHECK(str(0, {}) == "test string");
    CHECK(str(-5, {}) == "tring");
    CHECK(str(5u, {}) == "string");
    CHECK(str(1, {}) == "est string");
    CHECK(str(11, {}) == "");
    CHECK(str(11u, {}) == "");
    CHECK(str(0u, {}) == "test string");
    
    CHECK(str[bs::slice(2)] == "te");
    CHECK(str[bs::slice(0, 5)] == "test ");
    CHECK(str[bs::slice(5, 5)] == "");
    CHECK(str[bs::slice(5, 11)] == "string");
}

TEST_CASE("substr", "[string_view]") {
    CHECK("test string"_sv.substr(6) == "tring");
    CHECK("test string"_sv.substr(4, 3) == " st");
}

TEST_CASE("starts_with", "[string_view]") {
    CHECK("test string"_sv.starts_with("test") == true);
    CHECK("!hello"_sv.starts_with("hello!") == false);
    CHECK("test string"_sv.starts_with("test string") == true);
    CHECK("test string"_sv.starts_with('t') == true);
    CHECK("test string"_sv.starts_with('e') == false);
}

TEST_CASE("ends_with", "[string_view]") {
    CHECK("test string"_sv.ends_with("string") == true);
    CHECK("test string"_sv.ends_with("strin") == false);
    CHECK("test string"_sv.ends_with("test string") == true);
    CHECK("test string"_sv.ends_with('g') == true);
    CHECK("test string"_sv.ends_with('n') == false);
}

TEST_CASE("operators", "[string_view]") {
    const bs::string_view str = "test string";
    SECTION("==") {
        CHECK(str == str);
        CHECK(str == "test string");
        CHECK_FALSE(str == "hello");
    }
    SECTION("!=") {
        CHECK_FALSE(str != str);
        CHECK(str != "a");
        CHECK_FALSE(str != "test string");
    }
    SECTION(">") {
        CHECK_FALSE(str > str);
        CHECK(str > "a");
        CHECK_FALSE(str > "test string+");
        CHECK(str > "test strina");
    }
    SECTION(">=") {
        CHECK(str >= str);
        CHECK(str >= "b");
        CHECK_FALSE(str >= "test string+");
        CHECK(str >= "test strina");
    }
    SECTION("<") {
        CHECK_FALSE(str < str);
        CHECK(str < "test string!");
        CHECK_FALSE(str < "test-");
        CHECK(str < "test strinj");
    }
    SECTION("<=") {
        CHECK(str <= str);
        CHECK(str <= "test string!");
        CHECK_FALSE(str <= "aaa");
        CHECK(str <= "test strinj");
    }
}

TEST_CASE("find", "[string_view]") {
    const bs::string_view str = "test string";
    SECTION("string") {
        CHECK(str.find("test") == 0);
        CHECK(str.find("string") == 5);
        CHECK(str.find("test string") == 0);
        CHECK(str.find(" ") == 4);
        CHECK(str.find("e") == 1);
        CHECK(str.find("s", 3) == 5);

        CHECK(str.find("test string!!!") == str.size());
        CHECK(str.find("hello") == str.size());
        CHECK(str.find("") == 0);
        CHECK(str.find("", 3) == 3);
        CHECK(str.find("string", 8) == str.size());

        CHECK(str.find("str", 0, 4) == 4);
        CHECK(str.find("string", 5, 11) == 5);
    }
    SECTION("character") {
        CHECK(str.find('t') == 0);
        CHECK(str.find('g') == 10);
        CHECK(str.find('x') == str.size());
        CHECK(str.find('g', 11) == str.size());
        CHECK(str.find('r', 5, 8) == 7);

        CHECK(str.find('t').index() == 0);
        CHECK(str.find('g').index() == 10);
        CHECK(str.find('i').index_or_end() == 8);
        CHECK(str.find('k').index_or_end() == str.size());
        CHECK(str.find(' ').index_or_npos() == 4);
        CHECK(str.find('a').index_or_npos() == static_cast<std::size_t>(-1));

        CHECK_THROWS_AS(str.find('x').index_or_throw(), std::logic_error);
        CHECK_NOTHROW(str.find('t').index_or_throw());

        CHECK(str.find('t').ptr() == str.data() + 0);
        CHECK(str.find('o').ptr_or_end() == str.data() + str.size());
        CHECK(str.find('q').ptr_or_null() == nullptr);

        CHECK_THROWS_AS(str.find('x').ptr_or_throw(), std::logic_error);
        CHECK_NOTHROW(str.find(' ').ptr_or_throw());

        CHECK(str.find('e').found());
        CHECK_FALSE(str.find('p').found());

        CHECK(str.find('t', str.data() + 1).index() == 3);
        CHECK(str.find(" s", str.data() + 2).index() == 4);
    }
}

TEST_CASE("rfind", "[string_view]") {
    const bs::string_view str = "test string";

    SECTION("string") {
        CHECK(str.rfind("test") == 0);
        CHECK(str.rfind("string") == 5);
        CHECK(str.rfind(" ") == 4);
        CHECK(str.rfind("test", 8) == 0);
        CHECK(str.rfind("string", 10) == 0);
        CHECK(str.rfind("st", str.size(), 4) == 5);
    }
    SECTION("character") {
        CHECK(str.rfind(' ') == 4);
        CHECK(str.rfind('t') == 6);
        CHECK("hello"_sv.rfind('h') == 0);
        CHECK(str.rfind('g') == 10);
        CHECK(str.rfind('t', 3) == 0);
        CHECK(str.rfind('t', 11, 7) == 7);
        CHECK(str.rfind(' ', 7, 1) == 4);
    }
}

TEST_CASE("split", "[string_view]") {
    using Catch::Matchers::RangeEquals;

    SECTION("string") {
        CHECK_THAT("test string"_sv.split(" "), RangeEquals(std::array{"test", "string"}));
        CHECK_THAT("test string"_sv.split("t"), RangeEquals(std::array{"", "es", " s", "ring"}));
        CHECK_THAT("aaaa"_sv.split(" "), RangeEquals(std::array{"aaaa"}));

        CHECK("test string"_sv.split(" ")[0] =="test");
        CHECK("test string"_sv.split(" ")[1] == "string");
        CHECK("test string"_sv.split(" ").count() == 2);
        CHECK("  "_sv.split(" ")[1] == "");

        const auto splited_str = "test string"_sv.split(" ");
        CHECK_THAT(std::vector(splited_str.begin(), splited_str.end()), RangeEquals(std::array{"test", "string"}));
    }
    SECTION("character") {
        CHECK_THAT("test string"_sv.split(' '), RangeEquals(std::array{"test", "string"}));

        CHECK("test string"_sv.split('g').count() == 2);
        CHECK("test string"_sv.split('g')[0] == "test strin");
        CHECK("test string"_sv.split('g')[1] == "");
        CHECK_THAT("test string"_sv.split('g'), RangeEquals(std::array{"test strin", ""}));

        const auto splited_str = "test string"_sv.split(' ');
        CHECK_THAT(std::vector(splited_str.begin(), splited_str.end()), RangeEquals(std::array{"test", "string"}));
    }
}

TEST_CASE("strip", "[string_view]") {
    const std::array strs{" hello "_sv, "hello "_sv, " hello"_sv, "  hello"_sv, "hello  "_sv, "  hello  "_sv};
    for (const auto s : strs) {
        CAPTURE(s);
        CHECK(s.strip(' ') == "hello");
    }

    CHECK("test string"_sv.strip("test") == " string");
    CHECK("test string"_sv.strip("tes") == " string");
    CHECK(" \thellow\n "_sv.strip(" \t\n") == "hellow");
    CHECK("aaaaa"_sv.strip("a") == "");

    CHECK("test string"_sv.strip() == "test string");
}

TEST_CASE("lstrip", "[string_view]") {
    SECTION("character") {
        CHECK(" hello"_sv.lstrip(' ') == "hello");
        CHECK("  hello"_sv.lstrip(' ') == "hello");
        CHECK("hello "_sv.lstrip(' ') == "hello ");
        CHECK("hello  "_sv.lstrip(' ') == "hello  ");
        CHECK(" hello "_sv.lstrip(' ') == "hello ");
    }
    SECTION("string") {
        CHECK("test string"_sv.lstrip("test") == " string");
        CHECK("test string"_sv.lstrip("tes") == " string");
        CHECK("test string"_sv.lstrip("t") == "est string");
        CHECK("test string"_sv.lstrip("g") == "test string");
        CHECK("test string"_sv.lstrip("string") == "est string");

        CHECK("test string"_sv.lstrip() == "test string");
        CHECK(" \t \ntest"_sv.lstrip() == "test");
    }
}

TEST_CASE("rstrip", "[string_view]") {
    SECTION("string") {
        CHECK("hello "_sv.rstrip(' ') == "hello");
        CHECK("hello  "_sv.rstrip(' ') == "hello");
        CHECK(" hello"_sv.rstrip(' ') == " hello");
        CHECK(" hello "_sv.rstrip(' ') == " hello");

        CHECK("test string"_sv.rstrip("test") == "test string");
        CHECK("test string"_sv.rstrip("string") == "test ");
        CHECK("test string"_sv.rstrip("g") == "test strin");
        CHECK("test string"_sv.rstrip("t") == "test string");

        CHECK("test string"_sv.rstrip() == "test string");
        CHECK(" test \n \v"_sv.rstrip() == " test");
    }
}

TEST_CASE("contains", "[string_view]") {
    const bs::string_view str = "test string";

    SECTION("character") {
        CHECK(str.contains('t'));
        CHECK(str.contains(' '));
        CHECK(str.contains('g'));
        CHECK_FALSE(str.contains('h'));
        CHECK_FALSE(str.contains('o'));
    }
    SECTION("string") {
        CHECK(str.contains("t"));
        CHECK(str.contains("g"));
        CHECK(str.contains("test"));
        CHECK(str.contains("test string"));
        CHECK(str.contains("string"));
        CHECK_FALSE(str.contains("123"));
        CHECK_FALSE(str.contains("tes?"));
        CHECK_FALSE(str.contains("l"));
    }
}

TEST_CASE("count", "[string_view]") {
    const bs::string_view str = "test string";

    SECTION("character") {
        CHECK(str.count('t') == 3);
        CHECK(str.count(' ') == 1);
        CHECK(str.count('j') == 0);
        CHECK(str.count('g') == 1);
    }
    SECTION("string") {
        CHECK(str.count("test") == 1);
        CHECK(str.count("test string") == 1);
        CHECK(str.count("............") == 0);
        CHECK(str.count(" ") == 1);
        CHECK(str.count("t") == 3);
        CHECK(str.count("st") == 2);
        CHECK(str.count("") == str.size() + 1);
    }
}

TEST_CASE("find_first_of", "[string_view]") {
    const bs::string_view str = "test string";

    SECTION("character") {
        CHECK(str.find_first_of('t') == 0);
        CHECK(str.find_first_of('g') == 10);
        CHECK(str.find_first_of(' ') == 4);
    }
    SECTION("string") {
        CHECK(str.find_first_of("t") == 0);
        CHECK(str.find_first_of("g") == 10);
        CHECK(str.find_first_of("s ing") == 2);
        CHECK_FALSE(str.find_first_of("").found());
        CHECK_FALSE(str.find_first_of("l").found());
    }
}

TEST_CASE("find_last_of", "[string_view]") {
    const bs::string_view str = "test string";

    SECTION("character") {
        CHECK(str.find_last_of('t').index() == 6);
        CHECK(str.find_last_of('g').index() == 10);
        CHECK(str.find_last_of(' ').index() == 4);
    }
    SECTION("string") {
        CHECK(str.find_last_of("t") == 6);
        CHECK(str.find_last_of("g") == 10);
        CHECK(str.find_last_of("hei") == 8);
        CHECK(str.find_last_of("").ptr_or_null() == nullptr);
        CHECK(str.find_last_of("a").ptr_or_end() == str.data() + str.size());
    }
}

TEST_CASE("strip_first", "[string_view]") {
    const bs::string_view str = "test string";

    SECTION("character") {
        CHECK(str.strip_first('t') == "est string");
        CHECK(str.strip_first('e') == "test string");
        CHECK(str.strip_first('g') == "test string");
    }
    SECTION("string") {
        CHECK(str.strip_first("t") == "est string");
        CHECK(str.strip_first("test") == " string");
        CHECK(str.strip_first("string") == "test string");
        CHECK(str.strip_first("test string") == "");
        CHECK(str.strip_first("test string!") == "test string");
    }
}

TEST_CASE("strip_last", "[string_view]") {
    const bs::string_view str = "test string";

    SECTION("character") {
        CHECK(str.strip_last('g') == "test strin");
        CHECK(str.strip_last('t') == "test string");
        CHECK(str.strip_last('n') == "test string");
    }
    SECTION("string") {
        CHECK(str.strip_last("g") == "test strin");
        CHECK(str.strip_last("string") == "test ");
        CHECK(str.strip_last("test string") == "");
        CHECK(str.strip_last("!test string") == "test string");
        CHECK(str.strip_last("test") == "test string");
    }
}

TEST_CASE("idx", "[string_view]") {
    const bs::string_view str = "test string";

    CHECK(str.idx(str.data() + 2) == 2);
    CHECK(str.idx(str.dataend()) == str.size());
    CHECK(str.idx(str.data()) == 0);
}

TEST_CASE("all_of", "[string_view]") {
    CHECK("teststring"_sv.all_of(bs::ascii::is_alphabetic<char>));
    CHECK_FALSE("test string"_sv.all_of(bs::ascii::is_whitespace<char>));
    CHECK("12345"_sv.all_of(bs::ascii::is_digit<char>));
    CHECK(""_sv.all_of(bs::ascii::is_graphic<char>) == true);

    CHECK("test string"_sv.all_of(bs::ascii::is_ascii<char>));
    CHECK_FALSE(u"test\u0d9estring"_sv.all_of(bs::ascii::is_ascii<char16_t>));
    CHECK_FALSE(U"test\U0001f34cstring"_sv.all_of(bs::ascii::is_ascii<char32_t>));
}

TEST_CASE("any_of", "[string_view]") {
    CHECK("test string"_sv.any_of(bs::ascii::is_blank<char>));
    CHECK(""_sv.any_of(bs::ascii::is_alphanumeric<char>) == false);
}

TEST_CASE("none_of", "[string_view]") {
    CHECK("test string"_sv.none_of(bs::ascii::is_digit<char>));
    CHECK(""_sv.none_of(bs::ascii::is_graphic<char>) == true);
    CHECK_FALSE("test string"_sv.none_of(bs::ascii::is_blank<char>));
}

}
