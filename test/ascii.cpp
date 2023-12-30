#include <catch2/catch_test_macros.hpp>

#include <betterstring/ascii.hpp>

namespace {

TEST_CASE("is_digit", "[ascii]") {
    CHECK(bs::ascii::is_digit('0'));
    CHECK(bs::ascii::is_digit('1'));
    CHECK(bs::ascii::is_digit('8'));
    CHECK(bs::ascii::is_digit('9'));

    CHECK_FALSE(bs::ascii::is_digit('a'));
    CHECK_FALSE(bs::ascii::is_digit('/'));
    CHECK_FALSE(bs::ascii::is_digit(':'));

    CHECK(bs::ascii::is_digit(u'0'));
    CHECK(bs::ascii::is_digit(u'9'));
    CHECK_FALSE(bs::ascii::is_digit(u'\u0d9e'));

    CHECK(bs::ascii::is_digit(U'0'));
    CHECK(bs::ascii::is_digit(U'9'));
    CHECK_FALSE(bs::ascii::is_digit(U'\U0001f34c'));
}

TEST_CASE("is_hex_digit", "[ascii]") {
    CHECK(bs::ascii::is_hex_digit('0'));
    CHECK(bs::ascii::is_hex_digit('9'));
    CHECK(bs::ascii::is_hex_digit('A'));
    CHECK(bs::ascii::is_hex_digit('a'));
    CHECK(bs::ascii::is_hex_digit('F'));
    CHECK(bs::ascii::is_hex_digit('f'));

    CHECK_FALSE(bs::ascii::is_hex_digit('/'));
    CHECK_FALSE(bs::ascii::is_hex_digit(':'));
    CHECK_FALSE(bs::ascii::is_hex_digit('`'));
    CHECK_FALSE(bs::ascii::is_hex_digit('g'));

    CHECK(bs::ascii::is_digit(u'0'));
    CHECK_FALSE(bs::ascii::is_digit(u'\u0d9e'));
}

TEST_CASE("is_lower", "[ascii]") {
    CHECK(bs::ascii::is_lower('a'));
    CHECK(bs::ascii::is_lower('z'));
    CHECK(bs::ascii::is_lower('r'));

    CHECK_FALSE(bs::ascii::is_lower('A'));
    CHECK_FALSE(bs::ascii::is_lower('Z'));
    CHECK_FALSE(bs::ascii::is_lower('`'));
    CHECK_FALSE(bs::ascii::is_lower('{'));

    CHECK(bs::ascii::is_lower(u'a'));
    CHECK(bs::ascii::is_lower(U'z'));
}

TEST_CASE("is_upper", "[ascii]") {
    CHECK(bs::ascii::is_upper('A'));
    CHECK(bs::ascii::is_upper('Z'));
    CHECK_FALSE(bs::ascii::is_upper('a'));
    CHECK_FALSE(bs::ascii::is_upper('z'));
}

TEST_CASE("is_alpha", "[ascii]") {
    CHECK(bs::ascii::is_alpha('a'));
    CHECK(bs::ascii::is_alpha('b'));
    CHECK(bs::ascii::is_alpha('A'));
    CHECK(bs::ascii::is_alpha('B'));
    CHECK(bs::ascii::is_alpha('z'));
    CHECK(bs::ascii::is_alpha('Z'));

    CHECK_FALSE(bs::ascii::is_alpha('0'));
    CHECK_FALSE(bs::ascii::is_alpha('{'));
}

TEST_CASE("is_alnum", "[ascii]") {
    CHECK(bs::ascii::is_alnum('a'));
    CHECK(bs::ascii::is_alnum('b'));
    CHECK(bs::ascii::is_alnum('A'));
    CHECK(bs::ascii::is_alnum('B'));
    CHECK(bs::ascii::is_alnum('Z'));
    CHECK(bs::ascii::is_alnum('0'));
    CHECK(bs::ascii::is_alnum('1'));
    CHECK(bs::ascii::is_alnum('9'));

    CHECK_FALSE(bs::ascii::is_alnum('!'));
    CHECK_FALSE(bs::ascii::is_alnum('{'));
    CHECK_FALSE(bs::ascii::is_alnum('/'));
}

TEST_CASE("is_punct", "[ascii]") {
    CHECK(bs::ascii::is_punct('!'));
    CHECK(bs::ascii::is_punct('"'));
    CHECK(bs::ascii::is_punct('/'));
    CHECK(bs::ascii::is_punct(':'));
    CHECK(bs::ascii::is_punct('@'));
    CHECK(bs::ascii::is_punct('['));
    CHECK(bs::ascii::is_punct('`'));
    CHECK(bs::ascii::is_punct('{'));
    CHECK(bs::ascii::is_punct('~'));

    CHECK_FALSE(bs::ascii::is_punct('a'));
    CHECK_FALSE(bs::ascii::is_punct('b'));
    CHECK_FALSE(bs::ascii::is_punct('1'));
    CHECK_FALSE(bs::ascii::is_punct(' '));
}

TEST_CASE("is_graph", "[ascii]") {
    CHECK(bs::ascii::is_graph('!'));
    CHECK(bs::ascii::is_graph('~'));

    CHECK_FALSE(bs::ascii::is_graph(' '));
}

TEST_CASE("is_blank", "[ascii]") {
    CHECK(bs::ascii::is_blank('\t'));
    CHECK(bs::ascii::is_blank(' '));

    CHECK_FALSE(bs::ascii::is_blank('a'));
}

TEST_CASE("is_space", "[ascii]") {
    CHECK(bs::ascii::is_space('\t'));
    CHECK(bs::ascii::is_space('\r'));
    CHECK(bs::ascii::is_space(' '));

    CHECK_FALSE(bs::ascii::is_space('\10'));
    CHECK_FALSE(bs::ascii::is_space('\16'));
}

TEST_CASE("is_print", "[ascii]") {
    CHECK(bs::ascii::is_print(' '));
    CHECK(bs::ascii::is_print('~'));

    CHECK_FALSE(bs::ascii::is_print('\177'));
    CHECK_FALSE(bs::ascii::is_print('\37'));
}

TEST_CASE("is_ascii", "[ascii]") {
    CHECK(bs::ascii::is_ascii('\0'));
    CHECK(bs::ascii::is_ascii('a'));
    CHECK(bs::ascii::is_ascii('\177'));
    CHECK(bs::ascii::is_ascii(u'5'));
    CHECK(bs::ascii::is_ascii(U'o'));

    CHECK_FALSE(bs::ascii::is_ascii(static_cast<char>(130)));
    CHECK_FALSE(bs::ascii::is_ascii(static_cast<char>(-1)));
    CHECK_FALSE(bs::ascii::is_ascii(u'\u0d9e'));
    CHECK_FALSE(bs::ascii::is_ascii(U'\U0001f34c'));
}

TEST_CASE("to_lower", "[ascii]") {
    CHECK(bs::ascii::to_lower('a') == 'a');
    CHECK(bs::ascii::to_lower('A') == 'a');
    CHECK(bs::ascii::to_lower('z') == 'z');
    CHECK(bs::ascii::to_lower('Z') == 'z');
    CHECK(bs::ascii::to_lower('0') == '0');
    CHECK(bs::ascii::to_lower('9') == '9');
    CHECK(bs::ascii::to_lower('!') == '!');

    CHECK(bs::ascii::to_lower(u'a') == 'a');
    CHECK(bs::ascii::to_lower(u'A') == 'a');
    CHECK(bs::ascii::to_lower(U'a') == 'a');
    CHECK(bs::ascii::to_lower(U'A') == 'a');
}

TEST_CASE("to_upper", "[ascii]") {
    CHECK(bs::ascii::to_upper('a') == 'A');
    CHECK(bs::ascii::to_upper('A') == 'A');
    CHECK(bs::ascii::to_upper('b') == 'B');
    CHECK(bs::ascii::to_upper('B') == 'B');
    CHECK(bs::ascii::to_upper('Z') == 'Z');
    CHECK(bs::ascii::to_upper('z') == 'Z');
    CHECK(bs::ascii::to_upper('1') == '1');
    CHECK(bs::ascii::to_upper('!') == '!');
    CHECK(bs::ascii::to_upper('~') == '~');

    CHECK(bs::ascii::to_upper(u'a') == 'A');
    CHECK(bs::ascii::to_upper(u'A') == 'A');
    CHECK(bs::ascii::to_upper(U'a') == 'A');
    CHECK(bs::ascii::to_upper(U'A') == 'A');
}

TEST_CASE("to_digit", "[ascii]") {
    CHECK(bs::ascii::to_digit('0') == 0);
    CHECK(bs::ascii::to_digit('1') == 1);
    CHECK(bs::ascii::to_digit('9') == 9);
    CHECK(bs::ascii::to_digit(u'5') == 5);
}

TEST_CASE("to_hex_digit", "[ascii]") {
    CHECK(bs::ascii::to_hex_digit('0') == 0);
    CHECK(bs::ascii::to_hex_digit('9') == 9);
    CHECK(bs::ascii::to_hex_digit('A') == 10);
    CHECK(bs::ascii::to_hex_digit('a') == 10);
    CHECK(bs::ascii::to_hex_digit('b') == 11);
    CHECK(bs::ascii::to_hex_digit('B') == 11);
    CHECK(bs::ascii::to_hex_digit('f') == 15);
    CHECK(bs::ascii::to_hex_digit('F') == 15);
    CHECK(bs::ascii::to_hex_digit(u'5') == 5);
    CHECK(bs::ascii::to_hex_digit(u'c') == 12);
}

TEST_CASE("from_digit", "[ascii]") {
    CHECK(bs::ascii::from_digit(0) == '0');
    CHECK(bs::ascii::from_digit(1) == '1');
    CHECK(bs::ascii::from_digit(9) == '9');
    CHECK(bs::ascii::from_digit<char16_t>(5) == u'5');
    CHECK(bs::ascii::from_digit<char32_t>(7) == U'7');
}

TEST_CASE("from_lower_hex_digit", "[ascii]") {
    CHECK(bs::ascii::from_lower_hex_digit(0) == '0');
    CHECK(bs::ascii::from_lower_hex_digit(9) == '9');
    CHECK(bs::ascii::from_lower_hex_digit(10) == 'a');
    CHECK(bs::ascii::from_lower_hex_digit(11) == 'b');
    CHECK(bs::ascii::from_lower_hex_digit(15) == 'f');
    CHECK(bs::ascii::from_lower_hex_digit<char16_t>(12) == 'c');
    CHECK(bs::ascii::from_lower_hex_digit<char32_t>(13) == 'd');
}

TEST_CASE("from_upper_hex_digit", "[ascii]") {
    CHECK(bs::ascii::from_upper_hex_digit(0) == '0');
    CHECK(bs::ascii::from_upper_hex_digit(9) == '9');
    CHECK(bs::ascii::from_upper_hex_digit(10) == 'A');
    CHECK(bs::ascii::from_upper_hex_digit(11) == 'B');
    CHECK(bs::ascii::from_upper_hex_digit(15) == 'F');
    CHECK(bs::ascii::from_upper_hex_digit<char16_t>(12) == 'C');
    CHECK(bs::ascii::from_upper_hex_digit<char32_t>(13) == 'D');
}

}
