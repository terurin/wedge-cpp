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
    auto opt= parser(ss).opt_left();
    EXPECT_EQ(opt, 0);
}

}; // namespace constant_right_tests

