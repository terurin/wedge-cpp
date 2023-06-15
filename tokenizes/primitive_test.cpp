#include "primitive.hpp"
#include "concepts.hpp"
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
