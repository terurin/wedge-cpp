#include "primitive.hpp"
#include "repeats.hpp"
#include "gtest/gtest.h"
#include <sstream>

using namespace tokenizes::primitive;
using namespace tokenizes::repeats;
using namespace std;

namespace repeat_digit_tests {

const static auto parser = repeat(digit, 2, 4);

TEST(repeat, digit_1) {
    stringstream ss;
    ss << "1";
    EXPECT_EQ(parser(ss).opt_right(), std::nullopt);
}

TEST(repeat, digit_2) {
    stringstream ss;
    ss << "12";
    EXPECT_EQ(parser(ss).opt_right(), "12");
}

TEST(repeat, digit_4) {
    stringstream ss;
    ss << "1234";
    EXPECT_EQ(parser(ss).opt_right(), "1234");
}

TEST(repeat, digit_5) {
    stringstream ss;
    ss << "12345";
    EXPECT_EQ(parser(ss).opt_right(), "1234");
}

} // namespace repeat_digit

namespace many0_tests {
const static auto parser = many0(digit);

TEST(many0, digit_0) {
    stringstream ss;
    ss << "";
    EXPECT_EQ(parser(ss).opt_right(), "");
}

TEST(many0, digit_1) {
    stringstream ss;
    ss << "1";
    EXPECT_EQ(parser(ss).opt_right(), "1");
}

TEST(many0, digit_2) {
    stringstream ss;
    ss << "12";
    EXPECT_EQ(parser(ss).opt_right(), "12");
}

} // namespace many0

namespace many1_tests {
const static auto parser = many1(digit);

TEST(many1, digit_0) {
    stringstream ss;
    ss << "";
    EXPECT_EQ(parser(ss).opt_right(), std::nullopt);
}

TEST(many1, digit_1) {
    stringstream ss;
    ss << "1";
    EXPECT_EQ(parser(ss).opt_right(), "1");
}

TEST(many1, digit_2) {
    stringstream ss;
    ss << "12";
    EXPECT_EQ(parser(ss).opt_right(), "12");
}

} // namespace many0