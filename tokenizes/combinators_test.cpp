#include "combinators.hpp"
#include "primitive.hpp"
#include "gtest/gtest.h"
#include <algorithm>
#include <sstream>
using std::make_tuple;
using std::stringstream;
using tokenizes::primitive::digit;
using tokenizes::primitive::sign;
using namespace tokenizes::combinators;

namespace tupled_merge_tests {

// tuple version

TEST(typed_merge, tuple) {
    auto x = typed_merge(1, 'a');
    EXPECT_EQ(x, make_tuple(1, 'a'));
}

TEST(typed_merge, tuple_right) {
    std::tuple<int, char> x{1, 'a'};
    auto y = typed_merge(1, std::move(x));
    EXPECT_EQ(y, make_tuple(1, 1, 'a'));
}

TEST(typed_merge, tuple_left) {
    std::tuple<int, char> x{1, 'a'};
    auto y = typed_merge(std::move(x), 1);
    EXPECT_EQ(y, make_tuple(1, 'a', 1));
}

TEST(typed_merge, tuple_both) {
    std::tuple<int, char> x{1, 'a'};
    std::tuple<int, char> y{2, 'b'};
    auto z = typed_merge(std::move(x), std::move(y));
    EXPECT_EQ(z, make_tuple(1, 'a', 2, 'b'));
}

// vector version

TEST(typed_merge, vector) {
    auto ax = typed_merge(1, 2);
    const std::vector<int> bx = {1, 2};
    EXPECT_EQ(ax, bx);
}

TEST(typed_merge, vector_right) {
    std::vector<int> x{1, 2};
    auto y = typed_merge(0, std::move(x));
    const std::vector<int> z = {0, 1, 2};
    EXPECT_EQ(y, z);
}

TEST(typed_merge, vector_left) {
    std::vector<int> x{1, 2};
    auto y = typed_merge(std::move(x), 3);
    const std::vector<int> z = {1, 2, 3};
    EXPECT_EQ(y, z);
}

TEST(typed_merge, vector_both) {
    std::vector<int> x{1, 2};
    std::vector<int> y{3, 4};
    auto z = typed_merge(std::move(x), std::move(y));
    const std::vector<int> w = {1, 2, 3, 4};
    EXPECT_EQ(z, w);
}

// string version

TEST(typed_merge, string) {
    auto x = typed_merge('a', 'b');
    EXPECT_EQ(x, "ab");
}

TEST(typed_merge, string_right) {
    using std::string;
    auto x = typed_merge('a', string("bc"));
    EXPECT_EQ(x, "abc");
}

TEST(typed_merge, string_left) {
    using std::string;
    auto x = typed_merge(string("ab"), 'c');
    EXPECT_EQ(x, "abc");
}

TEST(typed_merge, string_both) {
    using std::string;
    auto x = typed_merge(string("ab"), string("cd"));
    EXPECT_EQ(x, "abcd");
}

// nullptr version

TEST(typed_merge, nullptr_right) {
    auto x = typed_merge(1, nullptr);
    EXPECT_EQ(x, 1);
}

TEST(typed_merge, nullptr_left) {
    auto x = typed_merge(nullptr, 1);
    EXPECT_EQ(x, 1);
}

TEST(typed_merge, nullptr_both) {
    auto x = typed_merge(nullptr, nullptr);
    EXPECT_EQ(x, nullptr);
}

} // namespace tupled_merge_tests

namespace sequencer2_tests {

const static auto parser = digit * digit;

TEST(sequencer, right_failed) {
    stringstream ss;
    ss << "xx";
    EXPECT_TRUE(parser(ss).is_left());
}

TEST(sequencer, left_failed) {
    stringstream ss;
    ss << "0x";
    EXPECT_TRUE(parser(ss).is_left());
}

TEST(sequencer, success) {
    stringstream ss;
    ss << "00";
    EXPECT_EQ(parser(ss).opt_right(), "00");
}

} // namespace sequencer2_tests

namespace sequencer3_tests {
const static auto parser = digit * digit * digit;

TEST(sequencer, failed_0) {
    stringstream ss;
    ss << "xxx";
    EXPECT_TRUE(parser(ss).is_left());
}

TEST(sequencer, failed_1) {
    stringstream ss;
    ss << "0xx";
    EXPECT_TRUE(parser(ss).is_left());
}

TEST(sequencer, failed_2) {
    stringstream ss;
    ss << "00x";
    EXPECT_TRUE(parser(ss).is_left());
}

TEST(sequencer, success) {
    stringstream ss;
    ss << "000";
    EXPECT_EQ(parser(ss).opt_right(), "000");
}

} // namespace sequencer3_tests

namespace branch_tests {
const static auto parser = sign + digit;

TEST(branch, sign) {
    stringstream ss;
    ss << "+";
    EXPECT_EQ(parser(ss).opt_right(), '+');
}

TEST(branch, digit) {
    stringstream ss;
    ss << "0";
    EXPECT_EQ(parser(ss).opt_right(), '0');
}

TEST(branch, others) {
    stringstream ss;
    ss << "x";
    EXPECT_EQ(parser(ss).opt_right(),std::nullopt);
}
} // namespace branch_tests
