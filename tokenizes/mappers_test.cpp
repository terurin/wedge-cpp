#include "mappers.hpp"
#include "primitive.hpp"
#include "gtest/gtest.h"
#include <sstream>

using namespace tokenizes::primitive;
using namespace tokenizes::mappers;
using namespace std;

namespace mapper_right_tests {
const static auto parser = mapper_right(digit, [](char c) { return (int)c - '0'; });

TEST(mapper_right, digit_success) {
    stringstream ss;

    ss << "0";
    EXPECT_EQ(parser(ss).opt_right(), 0);
}

TEST(mapper_right, digit_fail) {
    stringstream ss;

    ss << "x";
    EXPECT_EQ(parser(ss).opt_right(), nullopt);
}
} // namespace mapper_right_tests

namespace mapper_left_tests {
const static auto parser = mapper_left(digit, [](nullptr_t) { return 0; });

TEST(mapper_left, digit_success) {
    stringstream ss;

    ss << "0";
    EXPECT_EQ(parser(ss).opt_left(), nullopt);
}

TEST(mapper_left, digit_fail) {
    stringstream ss;

    ss << "a";
    EXPECT_EQ(parser(ss).opt_left(), 0);
}
} // namespace mapper_left_tests

namespace constant_right_tests {

const static auto parser = constant_right(digit, true);

TEST(constant_right, digit_success) {
    std::stringstream ss;

    ss << "0";
    EXPECT_EQ(parser(ss).opt_right(), true);
}

TEST(constant_right, digit_fail) {
    std::stringstream ss;

    ss << "x";
    EXPECT_EQ(parser(ss).opt_right(), nullopt);
}

}; // namespace constant_right_tests

namespace constant_left_tests {

const static auto parser = constant_left(digit, 0);

TEST(constant_left, digit_success) {
    std::stringstream ss;

    ss << "0";
    EXPECT_EQ(parser(ss).opt_left(), nullopt);
}

TEST(constant_left, digit_fail) {
    std::stringstream ss;

    ss << "x";
    EXPECT_EQ(parser(ss).opt_left(), 0);
}

}; // namespace constant_left_tests

namespace eraser_right_tests {

const static auto parser = eraser_right(digit);

// right: char -> nullptr, left: nullptr -> nullptr
TEST(eraser_right, digit_success) {
    std::stringstream ss;

    ss << "0";
    EXPECT_TRUE(parser(ss).is_right());
}

TEST(eraser_right, digit_fail) {
    std::stringstream ss;

    ss << "x";
    EXPECT_TRUE(parser(ss).is_left());
}

}; // namespace eraser_right_tests

namespace eraser_left_tests {

// right: char -> char, left: nullptr -> nullptr
const static auto parser = eraser_left(digit);

TEST(eraser_left, digit_success) {
    std::stringstream ss;
    
    ss << "0";
    EXPECT_EQ(parser(ss).opt_right(), '0');
}

TEST(eraser_left, digit_fail) {
    std::stringstream ss;

    ss << "x";
    EXPECT_TRUE(parser(ss).is_left());
}

}; // namespace eraser_left_tests


namespace recognition_tests {

// right: char -> char, left: nullptr -> nullptr
const static auto parser = recognition(digit);

TEST(recognition, digit_success) {
    std::stringstream ss;
    
    ss << "0";
    EXPECT_EQ(parser(ss).opt_right(), "0");
}

TEST(recognition, digit_fail) {
    std::stringstream ss;

    ss << "x";
    EXPECT_TRUE(parser(ss).is_left());
}

}; // namespace eraser_left_tests

namespace tag_mapper_tests {

const static tag_mapper<int> parser{{"one", 1}, {"two", 2}};

TEST(tag_mapper_tests, one) {
    std::stringstream ss;
    ss << "one";
    EXPECT_EQ(parser(ss).opt_right(), 1);
}

TEST(tag_mapper_tests, two) {
    std::stringstream ss;
    ss << "two";
    EXPECT_EQ(parser(ss).opt_right(), 2);
}

TEST(tag_mapper_tests, zero) {
    std::stringstream ss;
    ss << "zero";
    EXPECT_EQ(parser(ss).opt_right(), std::nullopt);
}

} // namespace tag_mapper_tests
