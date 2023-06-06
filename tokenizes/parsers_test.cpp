#include "parsers.hpp"
#include "gtest/gtest.h"
#include <sstream>
#include <string>
using namespace tokenizes;
using namespace std;

namespace map {

const static auto digit_parser = digit.map([](char c) { return (int)c - '0'; });

TEST(shell, map_success) {
    std::stringstream ss;
    ss << "0";
    EXPECT_EQ(digit_parser(ss), 0);
}

TEST(shell, map_fail) {
    std::stringstream ss;
    ss << "x";
    EXPECT_EQ(digit_parser(ss), nullopt);
}

} // namespace map

namespace opt_map {
const static auto digit_parser = digit.opt_map([](char c) -> std::optional<int> {
    if ('0' <= c && c <= '9') {
        return (int)c - '0';
    } else {
        return nullopt;
    }
});

TEST(shell, opt_map_success) {
    std::stringstream ss;
    ss << "0";
    EXPECT_EQ(digit_parser(ss), 0);
}

TEST(shell, opt_map_fail) {
    std::stringstream ss;
    ss << "x";
    EXPECT_EQ(digit_parser(ss), nullopt);
}

}