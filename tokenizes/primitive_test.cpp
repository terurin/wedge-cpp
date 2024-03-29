#include "concepts.hpp"
#include "primitive.hpp"
#include "gtest/gtest.h"
#include <sstream>
#include <string>
// using namespace tokenizes;
using namespace tokenizes::primitive;

namespace atom_tests {

TEST(atom, single_success) {
    const auto parser = atom('a');

    std::stringstream ss;
    ss << "a";
    EXPECT_EQ(parser(ss).opt_right(), 'a');
}

TEST(atom, single_fail) {
    const auto parser = atom('a');

    std::stringstream ss;
    ss << "b";
    EXPECT_EQ(parser(ss).opt_right(), std::nullopt);
}

TEST(atom, list) {
    const auto parser = atom("abc");

    std::stringstream ss;
    std::string s;

    ss << "a";
    EXPECT_EQ(parser(ss).opt_right(), 'a');

    ss << "b";
    EXPECT_EQ(parser(ss).opt_right(), 'b');

    ss << "c";
    EXPECT_EQ(parser(ss).opt_right(), 'c');

    ss << "d";
    EXPECT_EQ(parser(ss).opt_right(), std::nullopt);
}

TEST(atom, range) {
    const auto parser = atom::from_range('2', '8');
    std::stringstream ss;
    std::string s;
    ss << "2";
    EXPECT_EQ(parser(ss).opt_right(), '2');
    ss << "8";
    EXPECT_EQ(parser(ss).opt_right(), '8');
    ss << "1";
    EXPECT_EQ(parser(ss).opt_right(), std::nullopt);
    ss.ignore();
    ss << "9";
    EXPECT_EQ(parser(ss).opt_right(), std::nullopt);
}

TEST(atom, concept) {
    using tokenizes::concepts::parsable;
    EXPECT_TRUE(parsable<atom>);
}

} // namespace atom_tests

namespace tag_tests {
const static auto parser = tag("hello");
TEST(tag, hello_success) {
    std::stringstream ss;
    ss << "hello";
    EXPECT_EQ(parser(ss).opt_right(), "hello");
}

TEST(tag, hello_world_success) {
    std::stringstream ss;
    ss << "hello world";
    EXPECT_EQ(parser(ss).opt_right(), "hello");
}

TEST(tag, hello_fail) {
    std::stringstream ss;
    ss << "hell";
    const auto r = parser(ss).opt_right();
    EXPECT_EQ(r, std::nullopt);
}

TEST(tag, concept) {
    using tokenizes::concepts::parsable;
    EXPECT_TRUE(parsable<tag>);
}

} // namespace tag_tests

namespace tag_list_tests {

const static tag_list parser{"hello", "hello_world", "hola"};

TEST(tag_list, concept) {
    using tokenizes::concepts::parsable;
    EXPECT_TRUE(parsable<tag_list>);
}

TEST(tag_list, under_hello) {
    std::stringstream ss;
    ss << "hell";
    EXPECT_EQ(parser(ss).opt_right(), std::nullopt);
}

TEST(tag_list, just_hello) {
    std::stringstream ss;
    ss << "hello";
    EXPECT_EQ(parser(ss).opt_right(), "hello");
}

TEST(tag_list, over_hello) {
    std::stringstream ss;
    ss << "helloaa";
    EXPECT_EQ(parser(ss).opt_right(), "hello");
}

TEST(tag_list, hello_world) {
    std::stringstream ss;
    ss << "hello_world";
    EXPECT_EQ(parser(ss).opt_right(), "hello_world");
}

TEST(tag_list, hola) {
    std::stringstream ss;
    ss << "hola";
    EXPECT_EQ(parser(ss).opt_right(), "hola");
}

} // namespace tag_list_tests

namespace digit_parser_tests {

TEST(digit_parser, digit10_pass) {
    const auto parser = unsigned_parser();
    std::stringstream ss;
    ss << "9";
    const std::streampos pos = ss.tellg();
    EXPECT_EQ(parser(ss).opt_right(), 9);
    EXPECT_NE(pos, ss.tellg());
}

TEST(digit_parser, digit10_fail) {
    const auto parser = unsigned_parser();
    std::stringstream ss;
    ss << "A";
    const std::streampos pos = ss.tellg();
    EXPECT_EQ(parser(ss).opt_right(), std::nullopt);
    EXPECT_EQ(pos, ss.tellg());
}

TEST(digit_parser, digit16_pass) {
    const auto parser = unsigned_parser(16);
    std::stringstream ss;
    ss << "F";
    const std::streampos pos = ss.tellg();
    EXPECT_EQ(parser(ss).opt_right(), 0xf);
    EXPECT_NE(pos, ss.tellg());
}

TEST(digit_parser, digit16_fail) {
    const auto parser = unsigned_parser(16);
    std::stringstream ss;
    ss << "G";
    const std::streampos pos = ss.tellg();
    EXPECT_EQ(parser(ss).opt_right(), std::nullopt);
    EXPECT_EQ(pos, ss.tellg());
}

} // namespace digit_parser_tests

namespace unsigned_parser_tests {

TEST(unsigned_parser, base8_pass) {
    const auto parser = unsigned_parser(8);
    std::stringstream ss;
    ss << "17";
    EXPECT_EQ(parser(ss).opt_right(), 017);
}

TEST(unsigned_parser, base8_not_digit) {
    const auto parser = unsigned_parser(8);
    std::stringstream ss;
    ss << "8";
    EXPECT_EQ(parser(ss).opt_left(), unsigned_errors::not_digit);
}

TEST(unsigned_parser, base10_pass) {
    const auto parser = unsigned_parser();
    std::stringstream ss;
    ss << "10";
    EXPECT_EQ(parser(ss).opt_right(), 10);
}

TEST(unsigned_parser, base10_over) {
    const auto parser = unsigned_parser();
    std::stringstream ss;
    ss << "10#";
    EXPECT_EQ(parser(ss).opt_right(), 10);
}

TEST(unsigned_parser, base16_pass) {
    const auto parser = unsigned_parser(16);
    std::stringstream ss;
    ss << "1F";
    EXPECT_EQ(parser(ss).opt_right(), 0x1F);
}

TEST(unsigned_parser, base16_not_digit) {
    const auto parser = unsigned_parser(16);
    std::stringstream ss;
    ss << "G";
    EXPECT_EQ(parser(ss).opt_left(), unsigned_errors::not_digit);
}

TEST(unsigned_parser, base16_overflow) {
    const auto parser = unsigned_parser<uint8_t>(16);
    std::stringstream ss;
    ss << "100";
    EXPECT_EQ(parser(ss).opt_left(), unsigned_errors::overflow);
}

} // namespace unsigned_parser_tests

namespace signed_parser_tests {

TEST(signed_parser, base8_pass) {
    const auto parser = signed_parser(8);
    std::stringstream ss;
    ss << "17";
    EXPECT_EQ(parser(ss).opt_right(), 017);
}

TEST(signed_parser, base8_not_digit) {
    const auto parser = signed_parser(8);
    std::stringstream ss;
    ss << "8";
    EXPECT_EQ(parser(ss).opt_left(), signed_errors::not_digit);
}

TEST(signed_parser, base10_pass) {
    const auto parser = signed_parser();
    std::stringstream ss;
    ss << "10";
    EXPECT_EQ(parser(ss).opt_right(), 10);
}

TEST(signed_parser, base10_plus) {
    const auto parser = signed_parser();
    std::stringstream ss;
    ss << "+10";
    EXPECT_EQ(parser(ss).opt_right(), 10);
}

TEST(signed_parser, base10_minus) {
    const auto parser = signed_parser();
    std::stringstream ss;
    ss << "-10";
    EXPECT_EQ(parser(ss).opt_right(), -10);
}

TEST(signed_parser, base10_over) {
    const auto parser = signed_parser();
    std::stringstream ss;
    ss << "10#";
    EXPECT_EQ(parser(ss).opt_right(), 10);
}

TEST(signed_parser, base16_pass) {
    const auto parser = signed_parser(16);
    std::stringstream ss;
    ss << "1F";
    EXPECT_EQ(parser(ss).opt_right(), 0x1F);
}

TEST(signed_parser, base16_not_digit) {
    const auto parser = signed_parser(16);
    std::stringstream ss;
    ss << "G";
    EXPECT_EQ(parser(ss).opt_left(), signed_errors::not_digit);
}

TEST(signed_parser, base16_overflow_allow) {
    const auto parser = signed_parser<int8_t>(16);
    std::stringstream ss;
    ss << "7F";
    EXPECT_EQ(parser(ss).opt_left(), std::nullopt);
}

TEST(signed_parser, base16_overflow_deny) {
    const auto parser = signed_parser<int8_t>(16);
    std::stringstream ss;
    ss << "80";
    EXPECT_EQ(parser(ss).opt_left(), signed_errors::overflow);
}

TEST(signed_parser, base16_underflow_allow) {
    const auto parser = signed_parser<int8_t>(16);
    std::stringstream ss;
    ss << "-80";
    EXPECT_EQ(parser(ss).opt_left(), std::nullopt);
}

TEST(signed_parser, base16_underflow_deny) {
    const auto parser = signed_parser<int8_t>(16);
    std::stringstream ss;
    ss << "-81";
    EXPECT_EQ(parser(ss).opt_left(), signed_errors::underflow);
}

} // namespace signed_parser_tests

namespace integer_parser_tests {

TEST(integer_parser, base2_pass) {
    const auto parser = integer_parser();
    std::stringstream ss;
    ss << "0b1";
    EXPECT_EQ(parser(ss).opt_right(), 1);
}

TEST(integer_parser, base2_not_digit) {
    const auto parser = integer_parser();
    std::stringstream ss;
    ss << "0b2";
    EXPECT_EQ(parser(ss).opt_left(), integer_errors::not_digit);
}

TEST(integer_parser, base4_pass) {
    const auto parser = integer_parser();
    std::stringstream ss;
    ss << "0q3";
    EXPECT_EQ(parser(ss).opt_right(), 3);
}

TEST(integer_parser, base4_not_digit) {
    const auto parser = integer_parser();
    std::stringstream ss;
    ss << "0b4";
    EXPECT_EQ(parser(ss).opt_left(), integer_errors::not_digit);
}

TEST(integer_parser, base8_pass) {
    const auto parser = integer_parser();
    std::stringstream ss;
    ss << "0o7";
    EXPECT_EQ(parser(ss).opt_right(), 7);
}

TEST(integer_parser, base8_not_digit) {
    const auto parser = integer_parser();
    std::stringstream ss;
    ss << "0o8";
    EXPECT_EQ(parser(ss).opt_left(), integer_errors::not_digit);
}

TEST(integer_parser, base10_pass) {
    const auto parser = integer_parser();
    std::stringstream ss;
    ss << "10";
    EXPECT_EQ(parser(ss).opt_right(), 10);
}

TEST(integer_parser, base10_plus) {
    const auto parser = integer_parser();
    std::stringstream ss;
    ss << "+10";
    EXPECT_EQ(parser(ss).opt_right(), 10);
}

TEST(integer_parser, base10_minus) {
    const auto parser = integer_parser();
    std::stringstream ss;
    ss << "-10";
    EXPECT_EQ(parser(ss).opt_right(), -10);
}

TEST(integer_parser, base10_over) {
    const auto parser = integer_parser();
    std::stringstream ss;
    ss << "10#";
    EXPECT_EQ(parser(ss).opt_right(), 10);
}

TEST(integer_parser, base16_pass) {
    const auto parser = integer_parser();
    std::stringstream ss;
    ss << "0x1F";
    EXPECT_EQ(parser(ss).opt_right(), 0x1F);
}

TEST(integer_parser, base16_not_digit) {
    const auto parser = integer_parser();
    std::stringstream ss;
    ss << "0xG";
    EXPECT_EQ(parser(ss).opt_left(), integer_errors::not_digit);
}

TEST(integer_parser, base16_overflow_allow) {
    const auto parser = integer_parser<int8_t>();
    std::stringstream ss;
    ss << "0x7F";
    EXPECT_EQ(parser(ss).opt_left(), std::nullopt);
}

TEST(integer_parser, base16_overflow_deny) {
    const auto parser = integer_parser<int8_t>();
    std::stringstream ss;
    ss << "0x80";
    EXPECT_EQ(parser(ss).opt_left(), integer_errors::overflow);
}

TEST(integer_parser, base16_underflow_allow) {
    const auto parser = integer_parser<int8_t>();
    std::stringstream ss;
    ss << "-0x80";
    EXPECT_EQ(parser(ss).opt_left(), std::nullopt);
}

TEST(integer_parser, base16_underflow_deny) {
    const auto parser = integer_parser<int8_t>();
    std::stringstream ss;
    ss << "-0x81";
    EXPECT_EQ(parser(ss).opt_left(), integer_errors::underflow);
}

} // namespace integer_parser_tests

namespace string_parser_tests {
const static auto parser = string_parser("'");

TEST(string_parser, not_begin) {
    std::stringstream ss;
    ss << "x";
    EXPECT_EQ(parser(ss).opt_left(), string_errors::not_begin);
}

TEST(string_parser, hello) {
    std::stringstream ss;
    ss << "'hello'";
    EXPECT_EQ(parser(ss).opt_right(), "hello");
}

TEST(string_parser, bad_escape) {
    std::stringstream ss;
    ss << "'\\p'";
    EXPECT_EQ(parser(ss).opt_left(), string_errors::bad_escape);
}

TEST(string_parser, good_escape) {
    std::stringstream ss;
    ss << "'\\n'";
    EXPECT_EQ(parser(ss).opt_right(), "\n");
}

TEST(string_parser, not_end) {
    std::stringstream ss;
    ss << "'a";
    EXPECT_EQ(parser(ss).opt_left(), string_errors::not_end);
}

} // namespace string_parser_tests

namespace raw_string_parser_tests {
const static auto parser = raw_string_parser();

TEST(raw_string_parser, not_begin) {
    std::stringstream ss;
    ss << "x";
    EXPECT_EQ(parser(ss).opt_left(), raw_string_errors::not_begin);
}

TEST(raw_string_parser, hello) {
    std::stringstream ss;
    ss << "\"\"\"hello\"\"\"";
    EXPECT_EQ(parser(ss).opt_right(), "hello");
}

TEST(raw_string_parser, not_end) {
    std::stringstream ss;
    ss << "\"\"\"a";
    EXPECT_EQ(parser(ss).opt_left(), raw_string_errors::not_end);
}

} // namespace raw_string_parser_tests