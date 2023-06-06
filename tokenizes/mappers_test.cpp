#include "mappers.hpp"
#include "primitive.hpp"
#include "gtest/gtest.h"
#include <sstream>

using namespace tokenizes::primitive;
using namespace tokenizes::mappers;
using namespace std;

namespace mapper_tests {
const static auto digit_parser = mapper(digit, [](char c) { return (int)c - '0'; });

TEST(mapper, digit_success) {
    stringstream ss;

    ss << "0";
    EXPECT_EQ(digit_parser(ss), 0);
}

TEST(mapper, digit_fail) {
    stringstream ss;

    ss << "x";
    EXPECT_EQ(digit_parser(ss), nullopt);
}
} // namespace mapper_tests

namespace opt_tests {

const static auto digit_parser = opt_mapper(digit, [](char c) -> std::optional<int> {
    if ('0' <= c && c <= '9') {
        return (int)c - '0';
    } else {
        return nullopt;
    }
});

TEST(opt_mapper, digit_success) {
    std::stringstream ss;

    ss << "0";
    EXPECT_EQ(digit_parser(ss), 0);
}

TEST(opt_mapper, digit_fail) {
    std::stringstream ss;

    ss << "x";
    EXPECT_EQ(digit_parser(ss), nullopt);
}

} // namespace opt_tests

namespace to_value_tests {

const static auto digit_parser = to_value(digit, true);

TEST(to_value, digit_success) {
    std::stringstream ss;

    ss << "0";
    EXPECT_EQ(digit_parser(ss), true);
}

TEST(to_value, digit_fail) {
    std::stringstream ss;

    ss << "x";
    EXPECT_EQ(digit_parser(ss), nullopt);
}

}; // namespace to_value_tests
