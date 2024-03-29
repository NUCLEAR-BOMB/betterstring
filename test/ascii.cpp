
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>

#include "stacktrace.hpp"
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

TEST_CASE("is_hexdigit", "[ascii]") {
    CHECK(bs::ascii::is_hexdigit('0'));
    CHECK(bs::ascii::is_hexdigit('9'));
    CHECK(bs::ascii::is_hexdigit('A'));
    CHECK(bs::ascii::is_hexdigit('a'));
    CHECK(bs::ascii::is_hexdigit('F'));
    CHECK(bs::ascii::is_hexdigit('f'));

    CHECK_FALSE(bs::ascii::is_hexdigit('/'));
    CHECK_FALSE(bs::ascii::is_hexdigit(':'));
    CHECK_FALSE(bs::ascii::is_hexdigit('`'));
    CHECK_FALSE(bs::ascii::is_hexdigit('g'));

    CHECK(bs::ascii::is_hexdigit(u'0'));
    CHECK_FALSE(bs::ascii::is_hexdigit(u'\u0d9e'));
}

TEST_CASE("is_octdigit", "[ascii]") {
    CHECK(bs::ascii::is_octdigit('0'));
    CHECK(bs::ascii::is_octdigit('5'));
    CHECK(bs::ascii::is_octdigit('7'));
    CHECK_FALSE(bs::ascii::is_octdigit('8'));
}

TEST_CASE("is_lowercase", "[ascii]") {
    CHECK(bs::ascii::is_lowercase('a'));
    CHECK(bs::ascii::is_lowercase('z'));
    CHECK(bs::ascii::is_lowercase('r'));

    CHECK_FALSE(bs::ascii::is_lowercase('A'));
    CHECK_FALSE(bs::ascii::is_lowercase('Z'));
    CHECK_FALSE(bs::ascii::is_lowercase('`'));
    CHECK_FALSE(bs::ascii::is_lowercase('{'));

    CHECK(bs::ascii::is_lowercase(u'a'));
    CHECK(bs::ascii::is_lowercase(U'z'));
}

TEST_CASE("is_uppercase", "[ascii]") {
    CHECK(bs::ascii::is_uppercase('A'));
    CHECK(bs::ascii::is_uppercase('Z'));
    CHECK_FALSE(bs::ascii::is_uppercase('a'));
    CHECK_FALSE(bs::ascii::is_uppercase('z'));
}

TEST_CASE("is_alphabetic", "[ascii]") {
    CHECK(bs::ascii::is_alphabetic('a'));
    CHECK(bs::ascii::is_alphabetic('b'));
    CHECK(bs::ascii::is_alphabetic('A'));
    CHECK(bs::ascii::is_alphabetic('B'));
    CHECK(bs::ascii::is_alphabetic('z'));
    CHECK(bs::ascii::is_alphabetic('Z'));

    CHECK_FALSE(bs::ascii::is_alphabetic('0'));
    CHECK_FALSE(bs::ascii::is_alphabetic('{'));
}

TEST_CASE("is_alphanumeric", "[ascii]") {
    CHECK(bs::ascii::is_alphanumeric('a'));
    CHECK(bs::ascii::is_alphanumeric('b'));
    CHECK(bs::ascii::is_alphanumeric('A'));
    CHECK(bs::ascii::is_alphanumeric('B'));
    CHECK(bs::ascii::is_alphanumeric('Z'));
    CHECK(bs::ascii::is_alphanumeric('0'));
    CHECK(bs::ascii::is_alphanumeric('1'));
    CHECK(bs::ascii::is_alphanumeric('9'));

    CHECK_FALSE(bs::ascii::is_alphanumeric('!'));
    CHECK_FALSE(bs::ascii::is_alphanumeric('{'));
    CHECK_FALSE(bs::ascii::is_alphanumeric('/'));
}

TEST_CASE("is_punctuation", "[ascii]") {
    CHECK(bs::ascii::is_punctuation('!'));
    CHECK(bs::ascii::is_punctuation('"'));
    CHECK(bs::ascii::is_punctuation('/'));
    CHECK(bs::ascii::is_punctuation(':'));
    CHECK(bs::ascii::is_punctuation('@'));
    CHECK(bs::ascii::is_punctuation('['));
    CHECK(bs::ascii::is_punctuation('`'));
    CHECK(bs::ascii::is_punctuation('{'));
    CHECK(bs::ascii::is_punctuation('~'));

    CHECK_FALSE(bs::ascii::is_punctuation('a'));
    CHECK_FALSE(bs::ascii::is_punctuation('b'));
    CHECK_FALSE(bs::ascii::is_punctuation('1'));
    CHECK_FALSE(bs::ascii::is_punctuation(' '));
}

TEST_CASE("is_graphic", "[ascii]") {
    CHECK(bs::ascii::is_graphic('!'));
    CHECK(bs::ascii::is_graphic('~'));

    CHECK_FALSE(bs::ascii::is_graphic(' '));
}

TEST_CASE("is_blank", "[ascii]") {
    CHECK(bs::ascii::is_blank('\t'));
    CHECK(bs::ascii::is_blank(' '));

    CHECK_FALSE(bs::ascii::is_blank('a'));
}

TEST_CASE("is_whitespace", "[ascii]") {
    CHECK(bs::ascii::is_whitespace('\t'));
    CHECK(bs::ascii::is_whitespace('\r'));
    CHECK(bs::ascii::is_whitespace(' '));

    CHECK_FALSE(bs::ascii::is_whitespace('\10'));
    CHECK_FALSE(bs::ascii::is_whitespace('\16'));
}

TEST_CASE("is_printable", "[ascii]") {
    CHECK(bs::ascii::is_printable(' '));
    CHECK(bs::ascii::is_printable('~'));

    CHECK_FALSE(bs::ascii::is_printable('\177'));
    CHECK_FALSE(bs::ascii::is_printable('\37'));
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

TEST_CASE("to_lowercase", "[ascii]") {
    CHECK(bs::ascii::to_lowercase('a') == 'a');
    CHECK(bs::ascii::to_lowercase('A') == 'a');
    CHECK(bs::ascii::to_lowercase('z') == 'z');
    CHECK(bs::ascii::to_lowercase('Z') == 'z');
    CHECK(bs::ascii::to_lowercase('0') == '0');
    CHECK(bs::ascii::to_lowercase('9') == '9');
    CHECK(bs::ascii::to_lowercase('!') == '!');

    CHECK(bs::ascii::to_lowercase(u'a') == 'a');
    CHECK(bs::ascii::to_lowercase(u'A') == 'a');
    CHECK(bs::ascii::to_lowercase(U'a') == 'a');
    CHECK(bs::ascii::to_lowercase(U'A') == 'a');
}

TEST_CASE("to_uppercase", "[ascii]") {
    CHECK(bs::ascii::to_uppercase('a') == 'A');
    CHECK(bs::ascii::to_uppercase('A') == 'A');
    CHECK(bs::ascii::to_uppercase('b') == 'B');
    CHECK(bs::ascii::to_uppercase('B') == 'B');
    CHECK(bs::ascii::to_uppercase('Z') == 'Z');
    CHECK(bs::ascii::to_uppercase('z') == 'Z');
    CHECK(bs::ascii::to_uppercase('1') == '1');
    CHECK(bs::ascii::to_uppercase('!') == '!');
    CHECK(bs::ascii::to_uppercase('~') == '~');

    CHECK(bs::ascii::to_uppercase(u'a') == 'A');
    CHECK(bs::ascii::to_uppercase(u'A') == 'A');
    CHECK(bs::ascii::to_uppercase(U'a') == 'A');
    CHECK(bs::ascii::to_uppercase(U'A') == 'A');
}

TEST_CASE("to_digit", "[ascii]") {
    CHECK(bs::ascii::to_digit('0') == 0);
    CHECK(bs::ascii::to_digit('1') == 1);
    CHECK(bs::ascii::to_digit('9') == 9);
    CHECK(bs::ascii::to_digit(u'5') == 5);
}

TEST_CASE("to_hexdigit", "[ascii]") {
    CHECK(bs::ascii::to_hexdigit('0') == 0);
    CHECK(bs::ascii::to_hexdigit('9') == 9);
    CHECK(bs::ascii::to_hexdigit('A') == 10);
    CHECK(bs::ascii::to_hexdigit('a') == 10);
    CHECK(bs::ascii::to_hexdigit('b') == 11);
    CHECK(bs::ascii::to_hexdigit('B') == 11);
    CHECK(bs::ascii::to_hexdigit('f') == 15);
    CHECK(bs::ascii::to_hexdigit('F') == 15);
    CHECK(bs::ascii::to_hexdigit(u'5') == 5);
    CHECK(bs::ascii::to_hexdigit(u'c') == 12);
}

TEST_CASE("to_octdigit", "[ascii]") {
    CHECK(bs::ascii::to_octdigit('0') == 0);
    CHECK(bs::ascii::to_octdigit('1') == 1);
    CHECK(bs::ascii::to_octdigit('6') == 6);
    CHECK(bs::ascii::to_octdigit('7') == 7);
}

TEST_CASE("from_digit", "[ascii]") {
    CHECK(bs::ascii::from_digit(0) == '0');
    CHECK(bs::ascii::from_digit(1) == '1');
    CHECK(bs::ascii::from_digit(9) == '9');
    CHECK(bs::ascii::from_digit<char16_t>(5) == u'5');
    CHECK(bs::ascii::from_digit<char32_t>(7) == U'7');
}

TEST_CASE("from_lowercase_hexdigit", "[ascii]") {
    CHECK(bs::ascii::from_lowercase_hexdigit(0) == '0');
    CHECK(bs::ascii::from_lowercase_hexdigit(9) == '9');
    CHECK(bs::ascii::from_lowercase_hexdigit(10) == 'a');
    CHECK(bs::ascii::from_lowercase_hexdigit(11) == 'b');
    CHECK(bs::ascii::from_lowercase_hexdigit(15) == 'f');
    CHECK(bs::ascii::from_lowercase_hexdigit<char16_t>(12) == 'c');
    CHECK(bs::ascii::from_lowercase_hexdigit<char32_t>(13) == 'd');
}

TEST_CASE("from_uppercase_hexdigit", "[ascii]") {
    CHECK(bs::ascii::from_uppercase_hexdigit(0) == '0');
    CHECK(bs::ascii::from_uppercase_hexdigit(9) == '9');
    CHECK(bs::ascii::from_uppercase_hexdigit(10) == 'A');
    CHECK(bs::ascii::from_uppercase_hexdigit(11) == 'B');
    CHECK(bs::ascii::from_uppercase_hexdigit(15) == 'F');
    CHECK(bs::ascii::from_uppercase_hexdigit<char16_t>(12) == 'C');
    CHECK(bs::ascii::from_uppercase_hexdigit<char32_t>(13) == 'D');
}

TEST_CASE("from_octdigit", "[ascii]") {
    CHECK(bs::ascii::from_octdigit(0) == '0');
    CHECK(bs::ascii::from_octdigit(1) == '1');
    CHECK(bs::ascii::from_octdigit(6) == '6');
    CHECK(bs::ascii::from_octdigit(7) == '7');
}

TEST_CASE("ci_strcomp", "[ascii]") {
    CHECK(bs::ascii::ci_strcomp("helloworld", "helloworld", 10) == 0);
    CHECK(bs::ascii::ci_strcomp("HELLOWORLD", "HELLOWORLD", 10) == 0);
    CHECK(bs::ascii::ci_strcomp("test string", "test string", 10) == 0);
    CHECK(bs::ascii::ci_strcomp("TEST STRING", "TEST STRING", 0) == 0);
    CHECK(bs::ascii::ci_strcomp(".^#*!$@^%!#", ".^#*!$@^%!#", 11) == 0);
    CHECK(bs::ascii::ci_strcomp("     ", "     ", 5) == 0);
    CHECK(bs::ascii::ci_strcomp("", "", 0) == 0);

    CHECK(bs::ascii::ci_strcomp("a", "\0", 1) > 0);
    CHECK(bs::ascii::ci_strcomp("\0", "b", 1) < 0);
    CHECK(bs::ascii::ci_strcomp("a", "b", 1) < 0);
    CHECK(bs::ascii::ci_strcomp("aaaaa", "aaaab", 5) < 0);
    CHECK(bs::ascii::ci_strcomp("aaaac", "aaaab", 5) > 0);
    CHECK(bs::ascii::ci_strcomp("\0\0\0", "\0\0\0", 3) == 0);
    CHECK(bs::ascii::ci_strcomp("\0\0\0", "\0\0\1", 3) < 0);
    CHECK(bs::ascii::ci_strcomp("\0\0\2", "\0\0\1", 3) > 0);
    CHECK(bs::ascii::ci_strcomp("\0\0\2", "\0\3\1", 3) < 0);
    CHECK(bs::ascii::ci_strcomp("\0\4\2", "\0\3\1", 3) > 0);
    CHECK(bs::ascii::ci_strcomp("\0\4\2", "\5\3\1", 3) < 0);
    CHECK(bs::ascii::ci_strcomp("\6\4\2", "\5\3\1", 3) > 0);

    CHECK(bs::ascii::ci_strcomp("abcd", 4, "abce", 4) < 0);
    CHECK(bs::ascii::ci_strcomp("abf", 3, "abce", 4) < 0);
    CHECK(bs::ascii::ci_strcomp("abf", 3, "ab", 2) > 0);
}

}

