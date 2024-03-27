#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <array>
#include <iterator>

#include <betterstring/string_view.hpp>
#include <betterstring/ascii.hpp>

using namespace bs::literals;

namespace {

TEST_CASE("constructor", "[string_view]") {
    const bs::string_view empty_str;
    CHECK(empty_str.size() == 0);
    CHECK(empty_str.data() == nullptr);

    const bs::string_view copy_str("test string"_sv);
    CHECK(copy_str == "test string"_sv);

    const bs::string_view cut_str("test string", 4);
    CHECK(cut_str == "test"_sv);

    constexpr auto cstring = "123456789";
    const bs::string_view range_str(cstring + 1, cstring + 7);
    CHECK(range_str == "234567"_sv);
}

TEST_CASE("begin, end", "[string_view]") {
    const bs::string_view str = "test string"_sv;
    (void)str.begin();
    (void)str.cbegin();
    (void)str.crbegin();

    (void)str.end();
    (void)str.cend();
    (void)str.crend();
}

TEST_CASE("subscript", "[string_view]") {
    const bs::string_view str = "test string"_sv;

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
    const bs::string_view str = "test string"_sv;

    CHECK(str.front() == 't');
    CHECK(str.back() == 'g');
}

TEST_CASE("size, lenght", "[string_view]") {
    const bs::string_view str = "test string"_sv;
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
    bs::string_view str = "test string"_sv;
    str.remove_prefix(4);
    CHECK(str == " string"_sv);
}

TEST_CASE("remove_suffix", "[string_view]") {
    bs::string_view str = "test string"_sv;
    str.remove_suffix(3);
    CHECK(str == "test str"_sv);
}

TEST_CASE("slice", "[string_view]") {
    const bs::string_view str = "test string"_sv;
    CHECK(str(0, 4) == "test"_sv);
    CHECK(str(0, 4).size() == 4);
    
    CHECK(str(5, 11) == "string"_sv);
    CHECK(str(5u, 10u) == "strin"_sv);
    
    CHECK(str(0, -5) == "test s"_sv);
    CHECK(str(5, -2) == "stri"_sv);
    CHECK(str(7, -1) == "rin"_sv);
    CHECK(str(7u, -1) == "rin"_sv);
    CHECK(str(0, -1) == "test strin"_sv);

    CHECK(str(0, {}) == "test string"_sv);
    CHECK(str(-5, {}) == "tring"_sv);
    CHECK(str(5u, {}) == "string"_sv);
    CHECK(str(1, {}) == "est string"_sv);
    CHECK(str(11, {}) == ""_sv);
    CHECK(str(11u, {}) == ""_sv);
    CHECK(str(0u, {}) == "test string"_sv);
    
    CHECK(str[bs::slice(2)] == "te"_sv);
    CHECK(str[bs::slice(0, 5)] == "test "_sv);
    CHECK(str[bs::slice(5, 5)] == ""_sv);
    CHECK(str[bs::slice(5, 11)] == "string"_sv);
}

TEST_CASE("substr", "[string_view]") {
    CHECK("test string"_sv.substr(6) == "tring"_sv);
    CHECK("test string"_sv.substr(4, 3) == " st"_sv);
}

TEST_CASE("starts_with", "[string_view]") {
    CHECK("test string"_sv.starts_with("test"_sv) == true);
    CHECK("!hello"_sv.starts_with("hello!"_sv) == false);
    CHECK("test string"_sv.starts_with("test string"_sv) == true);
    CHECK("test string"_sv.starts_with('t') == true);
    CHECK("test string"_sv.starts_with('e') == false);
}

TEST_CASE("ends_with", "[string_view]") {
    CHECK("test string"_sv.ends_with("string"_sv) == true);
    CHECK("test string"_sv.ends_with("strin"_sv) == false);
    CHECK("test string"_sv.ends_with("test string"_sv) == true);
    CHECK("test string"_sv.ends_with('g') == true);
    CHECK("test string"_sv.ends_with('n') == false);
}

TEST_CASE("operators", "[string_view]") {
    const bs::string_view str = "test string"_sv;
    SECTION("==") {
        CHECK(str == str);
        CHECK(str == "test string"_sv);
        CHECK_FALSE(str == "hello"_sv);
    }
    SECTION("!=") {
        CHECK_FALSE(str != str);
        CHECK(str != "a"_sv);
        CHECK_FALSE(str != "test string"_sv);
    }
    SECTION(">") {
        CHECK_FALSE(str > str);
        CHECK(str > "a"_sv);
        CHECK_FALSE(str > "test string+"_sv);
        CHECK(str > "test strina"_sv);
    }
    SECTION(">=") {
        CHECK(str >= str);
        CHECK(str >= "b"_sv);
        CHECK_FALSE(str >= "test string+"_sv);
        CHECK(str >= "test strina"_sv);
    }
    SECTION("<") {
        CHECK_FALSE(str < str);
        CHECK(str < "test string!"_sv);
        CHECK_FALSE(str < "test-"_sv);
        CHECK(str < "test strinj"_sv);
    }
    SECTION("<=") {
        CHECK(str <= str);
        CHECK(str <= "test string!"_sv);
        CHECK_FALSE(str <= "aaa"_sv);
        CHECK(str <= "test strinj"_sv);
    }
}

TEST_CASE("find", "[string_view]") {
    const bs::string_view str = "test string"_sv;
    SECTION("string") {
        CHECK(str.find("test"_sv) == 0);
        CHECK(str.find("string"_sv) == 5);
        CHECK(str.find("test string"_sv) == 0);
        CHECK(str.find(" "_sv) == 4);
        CHECK(str.find("e"_sv) == 1);
        CHECK(str.find("s"_sv, 3) == 5);

        CHECK(str.find("test string!!!"_sv) == str.size());
        CHECK(str.find("hello"_sv) == str.size());
        CHECK(str.find(""_sv) == 0);
        CHECK(str.find(""_sv, 3) == 3);
        CHECK(str.find("string"_sv, 8) == str.size());

        CHECK(str.find("st ").index_opt() != std::nullopt);
        CHECK(str.find("abc").index_opt() == std::nullopt);
        CHECK(str.find(" string").index_opt() == 4);
        CHECK(str.find("test string").index_opt() == 0);

        CHECK(str.find("ing").ptr_opt() != std::nullopt);
        CHECK(str.find("ing").ptr_opt() == str.data() + 8);
        CHECK(str.find("xyz").ptr_opt() == std::nullopt);
    }
    SECTION("character") {
        CHECK(str.find('t') == 0);
        CHECK(str.find('g') == 10);
        CHECK(str.find('x') == str.size());
        CHECK(str.find('g', 11) == str.size());

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
        CHECK(str.find(" s"_sv, str.data() + 2).index() == 4);
    }
}

TEST_CASE("rfind", "[string_view]") {
    const bs::string_view str = "test string"_sv;

    SECTION("string") {
        CHECK(str.rfind("test"_sv) == 0);
        CHECK(str.rfind("string"_sv) == 5);
        CHECK(str.rfind(" "_sv) == 4);
        CHECK(str.rfind("test"_sv, 8) == 0);
        CHECK(str.rfind("string"_sv, 10) == -1);
    }
    SECTION("character") {
        CHECK(str.rfind(' ') == 4);
        CHECK(str.rfind('t') == 6);
        CHECK("hello"_sv.rfind('h') == 0);
        CHECK(str.rfind('g') == 10);
        CHECK(str.rfind('t', 3) == 0);
    }
}

TEST_CASE("split", "[string_view]") {
    using Catch::Matchers::RangeEquals;

    SECTION("string") {
        CHECK_THAT("test string"_sv.split(" "_sv), RangeEquals(std::array{"test"_sv, "string"_sv}));
        CHECK_THAT("test string"_sv.split("t"_sv), RangeEquals(std::array{""_sv, "es"_sv, " s"_sv, "ring"_sv}));
        CHECK_THAT("aaaa"_sv.split(" "_sv), RangeEquals(std::array{"aaaa"_sv}));

        CHECK("test string"_sv.split(" "_sv).nth(0) == "test"_sv);
        CHECK("test string"_sv.split(" "_sv).nth(1) == "string"_sv);
        CHECK("test string"_sv.split(" "_sv).count() == 2);
        CHECK("  "_sv.split(" "_sv).nth(1) == ""_sv);

        const auto splited_str = "test string"_sv.split(" "_sv);
        CHECK_THAT(std::vector(splited_str.begin(), splited_str.end()), RangeEquals(std::array{"test"_sv, "string"_sv}));

        const auto splited_str1 = "1 2 3 4 5 6 7 8 9"_sv.split(" "_sv);
        {
            auto it = splited_str1.begin();
            CHECK(*it == "1");
            std::advance(it, 3);
            CHECK(*it == "4");
        }
    }
    SECTION("character") {
        CHECK_THAT("test string"_sv.split(' '), RangeEquals(std::array{"test"_sv, "string"_sv}));

        CHECK("test string"_sv.split('g').count() == 2);
        CHECK("test string"_sv.split('g').nth(0) == "test strin"_sv);
        CHECK("test string"_sv.split('g').nth(1) == ""_sv);
        CHECK_THAT("test string"_sv.split('g'), RangeEquals(std::array{"test strin"_sv, ""_sv}));

        const auto splited_str = "test string"_sv.split(' ');
        CHECK_THAT(std::vector(splited_str.begin(), splited_str.end()), RangeEquals(std::array{"test"_sv, "string"_sv}));
    }
    SECTION("reverse split") {
        const auto split = "test string hello world"_sv.split(' ');
        {
            auto it = split.rbegin();
            CHECK(*it == "world");
            std::advance(it, 2);
            CHECK(*it == "string");
            ++it;
            CHECK(*it == "test");
            ++it;
            CHECK(it == split.rend());
        }
        CHECK_THAT(std::vector(split.rbegin(), split.rend()), RangeEquals(std::array{"world"_sv, "hello"_sv, "string"_sv, "test"_sv}));
    }
}

TEST_CASE("strip", "[string_view]") {
    const std::array strs{" hello "_sv, "hello "_sv, " hello"_sv, "  hello"_sv, "hello  "_sv, "  hello  "_sv};
    for (const auto s : strs) {
        CAPTURE(s);
        CHECK(s.strip(' ') == "hello"_sv);
    }

    CHECK("test string"_sv.strip("test"_sv) == " string"_sv);
    CHECK("test string"_sv.strip("tes"_sv) == " string"_sv);
    CHECK(" \thellow\n "_sv.strip(" \t\n"_sv) == "hellow"_sv);
    CHECK("aaaaa"_sv.strip("a"_sv) == ""_sv);
}

TEST_CASE("strip_left", "[string_view]") {
    SECTION("character") {
        CHECK(" hello"_sv.strip_left(' ') == "hello"_sv);
        CHECK("  hello"_sv.strip_left(' ') == "hello"_sv);
        CHECK("hello "_sv.strip_left(' ') == "hello "_sv);
        CHECK("hello  "_sv.strip_left(' ') == "hello  "_sv);
        CHECK(" hello "_sv.strip_left(' ') == "hello "_sv);
    }
    SECTION("string") {
        CHECK("test string"_sv.strip_left("test"_sv) == " string"_sv);
        CHECK("test string"_sv.strip_left("tes"_sv) == " string"_sv);
        CHECK("test string"_sv.strip_left("t"_sv) == "est string"_sv);
        CHECK("test string"_sv.strip_left("g"_sv) == "test string"_sv);
        CHECK("test string"_sv.strip_left("string"_sv) == "est string"_sv);
    }
}

TEST_CASE("strip_right", "[string_view]") {
    SECTION("string") {
        CHECK("hello "_sv.strip_right(' ') == "hello"_sv);
        CHECK("hello  "_sv.strip_right(' ') == "hello"_sv);
        CHECK(" hello"_sv.strip_right(' ') == " hello"_sv);
        CHECK(" hello "_sv.strip_right(' ') == " hello"_sv);

        CHECK("test string"_sv.strip_right("test"_sv) == "test string"_sv);
        CHECK("test string"_sv.strip_right("string"_sv) == "test "_sv);
        CHECK("test string"_sv.strip_right("g"_sv) == "test strin"_sv);
        CHECK("test string"_sv.strip_right("t"_sv) == "test string"_sv);
    }
}

TEST_CASE("contains", "[string_view]") {
    const bs::string_view str = "test string"_sv;

    SECTION("character") {
        CHECK(str.contains('t'));
        CHECK(str.contains(' '));
        CHECK(str.contains('g'));
        CHECK_FALSE(str.contains('h'));
        CHECK_FALSE(str.contains('o'));
    }
    SECTION("string") {
        CHECK(str.contains("t"_sv));
        CHECK(str.contains("g"_sv));
        CHECK(str.contains("test"_sv));
        CHECK(str.contains("test string"_sv));
        CHECK(str.contains("string"_sv));
        CHECK_FALSE(str.contains("123"_sv));
        CHECK_FALSE(str.contains("tes?"_sv));
        CHECK_FALSE(str.contains("l"_sv));
    }
}

TEST_CASE("count", "[string_view]") {
    const bs::string_view str = "test string"_sv;

    SECTION("character") {
        CHECK(str.count('t') == 3);
        CHECK(str.count(' ') == 1);
        CHECK(str.count('j') == 0);
        CHECK(str.count('g') == 1);
    }
    SECTION("string") {
        CHECK(str.count("test"_sv) == 1);
        CHECK(str.count("test string"_sv) == 1);
        CHECK(str.count("............"_sv) == 0);
        CHECK(str.count(" "_sv) == 1);
        CHECK(str.count("t"_sv) == 3);
        CHECK(str.count("st"_sv) == 2);
        CHECK(str.count(""_sv) == str.size() + 1);
    }
}

TEST_CASE("find_first_of", "[string_view]") {
    const bs::string_view str = "test string"_sv;

    SECTION("character") {
        CHECK(str.find_first_of('t') == 0);
        CHECK(str.find_first_of('g') == 10);
        CHECK(str.find_first_of(' ') == 4);
    }
    SECTION("string") {
        CHECK(str.find_first_of("t"_sv) == 0);
        CHECK(str.find_first_of("g"_sv) == 10);
        CHECK(str.find_first_of("s ing"_sv) == 2);
        CHECK_FALSE(str.find_first_of(""_sv).found());
        CHECK_FALSE(str.find_first_of("l"_sv).found());
    }
}

TEST_CASE("find_last_of", "[string_view]") {
    const bs::string_view str = "test string"_sv;

    SECTION("character") {
        CHECK(str.find_last_of('t').index() == 6);
        CHECK(str.find_last_of('g').index() == 10);
        CHECK(str.find_last_of(' ').index() == 4);
    }
    SECTION("string") {
        CHECK(str.find_last_of("t"_sv) == 6);
        CHECK(str.find_last_of("g"_sv) == 10);
        CHECK(str.find_last_of("hei"_sv) == 8);
        CHECK(str.find_last_of(""_sv).ptr_or_null() == nullptr);
        CHECK(str.find_last_of("a"_sv).ptr_or_end() == str.data() - 1);
        CHECK(str.find_last_of(" ").index_or(10) == 4);
        CHECK(str.find_last_of("y").index_or(5) == 5);
        CHECK(str.find_last_of("ten ").ptr_or(str.data() + 5) == str.data() + 9);
        CHECK(str.find_last_of("xqqq").ptr_or(str.data() - 1) == str.data() - 1);
    }
}

TEST_CASE("strip_first", "[string_view]") {
    const bs::string_view str = "test string"_sv;

    SECTION("character") {
        CHECK(str.strip_first('t') == "est string"_sv);
        CHECK(str.strip_first('e') == "test string"_sv);
        CHECK(str.strip_first('g') == "test string"_sv);
    }
    SECTION("string") {
        CHECK(str.strip_first("t"_sv) == "est string"_sv);
        CHECK(str.strip_first("test"_sv) == " string"_sv);
        CHECK(str.strip_first("string"_sv) == "test string"_sv);
        CHECK(str.strip_first("test string"_sv) == ""_sv);
        CHECK(str.strip_first("test string!"_sv) == "test string"_sv);
    }
}

TEST_CASE("strip_last", "[string_view]") {
    const bs::string_view str = "test string"_sv;

    SECTION("character") {
        CHECK(str.strip_last('g') == "test strin"_sv);
        CHECK(str.strip_last('t') == "test string"_sv);
        CHECK(str.strip_last('n') == "test string"_sv);
    }
    SECTION("string") {
        CHECK(str.strip_last("g"_sv) == "test strin"_sv);
        CHECK(str.strip_last("string"_sv) == "test "_sv);
        CHECK(str.strip_last("test string"_sv) == ""_sv);
        CHECK(str.strip_last("!test string"_sv) == "test string"_sv);
        CHECK(str.strip_last("test"_sv) == "test string"_sv);
    }
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

TEST_CASE("find_first_not_of", "[string_view]") {
    const bs::string_view str = "test string";
    CHECK(str.find_first_not_of('t') == 1);
    CHECK(str.find_first_not_of('e') == 0);
    CHECK_FALSE(""_sv.find_first_not_of('a').found());

    CHECK(str.find_first_not_of("test") == 4);
    CHECK(str.find_first_not_of("tes") == 4);
    CHECK(str.find_first_not_of("tes ") == 7);
    CHECK(str.find_first_not_of("").found());
    CHECK_FALSE(""_sv.find_first_not_of("daksdh,aw").found());
}

TEST_CASE("find_last_not_of", "[string_view]") {
    const bs::string_view str = "hello worldddd";
    CHECK(str.find_last_not_of('d') == 9);
    CHECK(str.find_last_not_of('l') == 13);
    CHECK_FALSE(""_sv.find_last_not_of('a').found());

    CHECK(str.find_last_not_of("dddd") == 9);
    CHECK(str.find_last_not_of("dlorw") == 5);
    CHECK(str.find_last_not_of("").found());
    CHECK_FALSE(""_sv.find_first_not_of("4q34jtk").found());
}

}
