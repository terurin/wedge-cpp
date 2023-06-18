#include "combinators.hpp"
#include "primitive.hpp"
#include "gtest/gtest.h"
#include <algorithm>
#include <sstream>
using std::make_tuple;
using std::stringstream;
using tokenizes::primitive::digit;
using namespace tokenizes::combinators;

namespace tupled_merge_tests {

TEST(typed_merge, tuple) {
    auto x = typed_merge(1, 'a');
    auto yr = typed_merge(1, std::move(x));
    auto yl = typed_merge(std::move(x), 1);
    auto z = typed_merge(std::move(x), std::move(x));

    EXPECT_EQ(x, make_tuple(1, 'a'));
    EXPECT_EQ(yr, make_tuple(1, 1, 'a'));
    EXPECT_EQ(yl, make_tuple(1, 'a', 1));
    EXPECT_EQ(z, make_tuple(1, 'a', 1, 'a'));
}

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

} // namespace tupled_merge_tests

namespace sequencer_tests {

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
    EXPECT_TRUE(parser(ss).is_right());
    // EXPECT_EQ(parser(ss).opt_right(),std::string("00"));
}

} // namespace sequencer_tests

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
    EXPECT_TRUE(parser(ss).is_right());
}

} // namespace sequencer3_tests