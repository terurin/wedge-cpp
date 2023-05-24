#include "parsers.hpp"
#include "gtest/gtest.h"
#include <sstream>
#include <string>
using namespace tokenizes;
using namespace std;

TEST(atom, char) {
    const auto parser = atom::from('a');

    stringstream ss;
    std::string s;
    empty_t empty;
    ss << "a";
    EXPECT_TRUE((*parser)(ss, s, empty));

    ss << "b";
    EXPECT_FALSE((*parser)(ss, s, empty));
}

TEST(atom, list) {
    const auto parser = atom::from("abc");

    stringstream ss;
    std::string s;
    empty_t empty;

    ss << "a";
    EXPECT_TRUE((*parser)(ss, s, empty));

    ss << "b";
    EXPECT_TRUE((*parser)(ss, s, empty));

    ss << "c";
    EXPECT_TRUE((*parser)(ss, s, empty));

    ss << "d";
    EXPECT_FALSE((*parser)(ss, s, empty));
}

TEST(atom, range) {

    const auto parser = atom::from_range('2', '8');
    stringstream ss;
    std::string s;
    empty_t empty;
    ss << "2";
    EXPECT_TRUE((*parser)(ss, s, empty));
    ss << "8";
    EXPECT_TRUE((*parser)(ss, s, empty));
    ss << "1";
    EXPECT_FALSE((*parser)(ss, s, empty)), ss.ignore();
    ss << "9";
    EXPECT_FALSE((*parser)(ss, s, empty));
}
