#include "primitive.hpp"
#include "repeats.hpp"
#include "gtest/gtest.h"
#include <sstream>
using namespace tokenizes::eithers;

TEST(either, right_prop) {
    either<int, std::string> e = right(1);

    EXPECT_EQ(e.get_mode(), either_mode::right);
    EXPECT_FALSE(e.is_none());
    EXPECT_TRUE(e.is_right());
    EXPECT_FALSE(e.is_left());

    // opt
    EXPECT_EQ(e.opt_right(), 1);
    EXPECT_EQ(e.opt_left(), std::nullopt);

    // get
    EXPECT_EQ(e.get_right(), 1);
    EXPECT_THROW(e.get_left(), std::out_of_range);

    // get-*-or
    EXPECT_EQ(e.get_right_or(2), 1);
    EXPECT_EQ(e.get_left_or("abc"), "abc");
}

TEST(either, left_prop) {
    using namespace std::string_view_literals;
    either<int, std::string> e =left("abc"sv);

    EXPECT_EQ(e.get_mode(), either_mode::left);
    EXPECT_FALSE(e.is_none());
    EXPECT_FALSE(e.is_right());
    EXPECT_TRUE(e.is_left());

    // opt
    EXPECT_EQ(e.opt_right(), std::nullopt);
    EXPECT_EQ(e.opt_left(), "abc");

    // get
    EXPECT_THROW(e.get_right(), std::out_of_range);
    EXPECT_EQ(e.get_left(), "abc");

    // get-*-or
    EXPECT_EQ(e.get_right_or(2), 2);
    EXPECT_EQ(e.get_left_or("xyz"), "abc");
}

TEST(either, reset) {
    either<int, std::string> e = right(1);
    e.reset();
    EXPECT_TRUE(e.is_none());
    EXPECT_FALSE(e.is_right());
    EXPECT_FALSE(e.is_left());
}

TEST(either, assign_either_right) {
    either<int, std::string> e = right(1);
    either<int, std::string> f = std::move(e);
    EXPECT_EQ(f.opt_right(), 1);
}

TEST(either, assign_either_left) {
    either<int, std::string> e = left<std::string>("abc");
    either<int, std::string> f = std::move(e);

    EXPECT_EQ(f.opt_left(), "abc");
}

TEST(either, assign_left) {
    either<int, std::string> e;
    e = left<std::string>("abc");
    EXPECT_EQ(e.opt_left(), "abc");
}

TEST(either, assign_right) {
    either<int, std::string> e;
    e = right(1);
    EXPECT_EQ(e.opt_right(), 1);
}

TEST(either, map_right) {
    either<int, std::string> e = right(10);
    auto f = e.map_right([](const int &x) { return x + 10; });
    EXPECT_EQ(f.opt_right(), 20);
}

TEST(either, map_left) {
    either<int, std::string> e = left<std::string>("a");
    auto f = e.map_left([](const std::string &x) { return x + "b"; });
    EXPECT_EQ(f.opt_left(), "ab");
}
