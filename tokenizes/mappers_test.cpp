#include "mappers.hpp"
#include "primitive.hpp"
#include "gtest/gtest.h"
#include <sstream>

using namespace tokenizes::primitive;
using namespace tokenizes::mappers;
using namespace std;

namespace mapper_tests {
const static auto digit_parser = mapper(digit, [](char c) { return (int)c - '0'; });

TEST(mapper, digit_success) {
    stringstream ss;

    ss << "0";
    EXPECT_EQ(digit_parser(ss).opt_right(), 0);
}

TEST(mapper, digit_fail) {
    stringstream ss;

    ss << "x";
    EXPECT_EQ(digit_parser(ss).opt_right(), nullopt);
}
} // namespace mapper_tests

namespace to_value_tests {

const static auto digit_parser = to_value(digit, true);

TEST(to_value, digit_success) {
    std::stringstream ss;

    ss << "0";
    EXPECT_EQ(digit_parser(ss).opt_right(), true);
}

TEST(to_value, digit_fail) {
    std::stringstream ss;

    ss << "x";
    EXPECT_EQ(digit_parser(ss).opt_right(), nullopt);
}

}; // namespace to_value_tests
