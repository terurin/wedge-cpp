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

namespace digits_tests {

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

} // namespace digits_tests