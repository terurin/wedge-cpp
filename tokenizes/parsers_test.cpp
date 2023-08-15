#include "parsers.hpp"
#include "gtest/gtest.h"
#include <sstream>
#include <string>
using namespace tokenizes;
using namespace std;

namespace map {

const static auto digit_parser = digit.map_right([](char c) { return (int)c - '0'; });

TEST(shell, map_success) {
    std::stringstream ss;
    ss << "0";
    EXPECT_EQ(digit_parser(ss).opt_right(), 0);
}

TEST(shell, map_fail) {
    std::stringstream ss;
    ss << "x";
    EXPECT_EQ(digit_parser(ss).opt_right(), std::nullopt);
}

} // namespace map

namespace constant_tests {
const static auto parser = digit.const_right(true);
TEST(shell, constant_success) {

    std::stringstream ss;
    ss << "0";
    EXPECT_EQ(parser(ss).opt_right(), true);
}

TEST(shell, constant_fail) {

    std::stringstream ss;
    ss << "x";
    EXPECT_EQ(parser(ss).opt_right(), nullopt);
}

}; // namespace constant_tests

namespace tag_mapper_tests {
const static auto parser = tokenizes::tag_mapper<int>({{"1",1},{"2",2}});
TEST(shell, tag_mapper_zero) {

    std::stringstream ss;
    ss << "0";
    EXPECT_EQ(parser(ss).opt_right(), std::nullopt);
}

TEST(shell, tag_mapper_one) {

    std::stringstream ss;
    ss << "1";
    EXPECT_EQ(parser(ss).opt_right(), 1);
}

TEST(shell, tag_mapper_two) {

    std::stringstream ss;
    ss << "2";
    EXPECT_EQ(parser(ss).opt_right(), 2);
}

}; // namespace constant_tests

