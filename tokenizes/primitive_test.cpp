#include "primitive.hpp"
#include "gtest/gtest.h"
#include <sstream>
#include <string>
//using namespace tokenizes;
using namespace tokenizes::primitive;
using namespace std;

TEST(atom, single_success) {
    const auto parser = atom('a');

    stringstream ss;
    ss << "a";
    EXPECT_EQ(parser(ss), 'a');
}

TEST(atom, single_fail) {
    const auto parser = atom('a');

    stringstream ss;
    ss << "b";
    EXPECT_EQ(parser(ss), std::nullopt);
}

TEST(atom, list) {
    const auto parser = atom("abc");

    stringstream ss;
    std::string s;

    ss << "a";
    EXPECT_EQ(parser(ss), 'a');

    ss << "b";
    EXPECT_EQ(parser(ss), 'b');

    ss << "c";
    EXPECT_EQ(parser(ss), 'c');

    ss << "d";
    EXPECT_EQ(parser(ss), std::nullopt);
}

TEST(atom, range) {

    const auto parser = atom::from_range('2', '8');
    stringstream ss;
    std::string s;
    ss << "2";
    EXPECT_EQ(parser(ss), '2');
    ss << "8";
    EXPECT_EQ(parser(ss), '8');
    ss << "1";
    EXPECT_EQ(parser(ss), std::nullopt);
    ss.ignore();
    ss << "9";
    EXPECT_EQ(parser(ss), std::nullopt);
}
