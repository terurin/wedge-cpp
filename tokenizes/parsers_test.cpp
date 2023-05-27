#include "parsers.hpp"
#include "gtest/gtest.h"
#include <sstream>
#include <string>
using namespace tokenizes;
using namespace std;

TEST(atom, char) {
    const auto parser = atom::create('a');

    stringstream ss;
    std::string s;
    empty_t empty;
    ss << "a";
    EXPECT_TRUE((*parser)(ss, s, empty));

    ss << "b";
    EXPECT_FALSE((*parser)(ss, s, empty));
}

TEST(atom, list) {
    const auto parser = atom::create("abc");

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

    const auto parser = atom::create_range('2', '8');
    stringstream ss;
    std::string s;
    empty_t empty;
    ss << "2";
    EXPECT_TRUE((*parser)(ss, s));
    ss << "8";
    EXPECT_TRUE((*parser)(ss, s));
    ss << "1";
    EXPECT_FALSE((*parser)(ss, s)), ss.ignore();
    ss << "9";
    EXPECT_FALSE((*parser)(ss, s));
}

TEST(repeat, repeat) {
    // auto p = base<string, empty_t>::from();
    const auto parser = repeat_n_m(atom::create_range('0', '9'), 2, 4);
    stringstream ss;
    std::string s;

    // 1
    s.clear();
    ss.str("0");
    EXPECT_FALSE((*parser)(ss, s));

    // 2
    s.clear();
    ss.clear();
    ss.str("01");
    EXPECT_TRUE((*parser)(ss, s) && s == "01");

    // 4
    s.clear();
    ss.clear();
    ss.str("0123");
    EXPECT_TRUE((*parser)(ss, s) && s == "0123");

    // 5
    s.clear();
    ss.clear();
    ss.str("01234");
    EXPECT_TRUE((*parser)(ss, s) && s == "0123");
}

TEST(mapper, mapper) {
    const auto parser = atom::create("1")->map([](const std::string &in) { return 1; });
    stringstream ss;
    int out;

    // success
    ss.str("1");
    EXPECT_TRUE((*parser)(ss, out) && out == 1);

    // failed
    ss.clear();
    ss.str("0");
    EXPECT_FALSE((*parser)(ss, out));
}

TEST(error_mapper, error_mapper) {

    const auto parser = atom::create("1")->map_err([](const empty_t &in) -> int { return 1; });

    stringstream ss;
    string r;
    int l;

    // fail
    l = 0;
    ss.str("0");
    EXPECT_FALSE((*parser)(ss, r, l) && l == 1);

    // success
    l = 0;
    ss.str("1");
    EXPECT_TRUE((*parser)(ss, r, l));
}

TEST(tag, tag) {
    const auto parser = tag::create("abc");
    stringstream ss;
    string r;

    // success
    ss.str("abc");
    EXPECT_TRUE((*parser)(ss, r) && r == "abc");

    // fail (all mismatch)
    r.clear();
    ss.clear();
    ss.str("xyz");
    EXPECT_FALSE((*parser)(ss, r));
    EXPECT_EQ(r,"");

    r.clear();
    ss.clear();
    ss.str("abz");
    EXPECT_FALSE((*parser)(ss, r));
    EXPECT_EQ(r,"");
}
