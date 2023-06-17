#include "combinators.hpp"
#include "primitive.hpp"
#include "gtest/gtest.h"
#include <sstream>

using std::make_tuple;
using std::stringstream;
using tokenizes::primitive::digit;
using namespace tokenizes::combinators;

namespace tupled_merge_tests {

TEST(tupled_merge, type) {
    auto x = tupled_merge(1, 1);
    auto yr = tupled_merge(1, x);
    auto yl = tupled_merge(x, 1);
    auto z = tupled_merge(x, x);

    EXPECT_EQ(x, make_tuple(1, 1));
    EXPECT_EQ(yr, make_tuple(1, 1, 1));
    EXPECT_EQ(yl, make_tuple(1, 1, 1));
    EXPECT_EQ(z, make_tuple(1, 1, 1, 1));
}

} // namespace tupled_merge_tests

namespace sequencer_tests {

const static auto parser = sequencer(digit, digit);

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
    EXPECT_EQ(parser(ss).opt_right(), std::make_tuple('0', '0'));
}

} // namespace sequencer_tests

namespace sequencer3_tests {
const static auto parser = sequencer(digit, sequencer(digit, digit));

TEST(sequencer, success) {
    stringstream ss;
    ss << "000";
    EXPECT_EQ(parser(ss).opt_right(), std::make_tuple('0', '0', '0'));
}

} // namespace sequencer3_tests