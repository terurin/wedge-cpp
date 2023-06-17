#include "combinators.hpp"
#include "primitive.hpp"
#include "gtest/gtest.h"
#include <sstream>

using std::stringstream;
using tokenizes::primitive::digit;
using namespace tokenizes::combinators;
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