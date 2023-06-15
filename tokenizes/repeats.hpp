#pragma once

#include "concepts.hpp"
#include "either.hpp"
#include <functional>
#include <istream>
#include <optional>
#include <vector>
namespace tokenizes::repeats {
using tokenizes::concepts::either_of;
using tokenizes::concepts::left_of;
using tokenizes::concepts::parsable;
using tokenizes::concepts::right_of;
using tokenizes::eithers::either;
using tokenizes::eithers::either_mode;
using tokenizes::eithers::left;
using tokenizes::eithers::right;
template <parsable P, class C>
class repeat {
public:
    using right_t = C;
    using left_t = left_of<P>;

private:
    P parser;
    size_t n, m;

public:
    repeat(const P &_parser, size_t _n = 0, size_t _m = SIZE_MAX) : parser(_parser), n(_n), m(_m) {}
    either<C, left_t> operator()(std::istream &is) const {
        size_t i = 0;
        C items;
        // head
        for (const auto head = is.tellg(); i < n; i++) {
            const either_of<P> item = parser(is);
            switch (item.get_mode()) {
            case either_mode::right:
                items.push_back(item.get_right());
                break;
            case either_mode::left:
                is.seekg(head);
                return left<left_t>(item.get_left());
            case either_mode::none:
                throw std::range_error("none is unexpceted");
            default:
                throw std::range_error("others is unexpceted");
            }
        }

        // tail
        for (; i < m; i++) {
            const auto tail = is.tellg();
            const either_of<P> item = parser(is);
            if (!item.is_right()) {
                is.seekg(tail);
                return right<C>(items);
            }
            items.push_back(item.get_right());
        }
        return right<C>(items);
    }
};

// T -> vector<T>
template <class P>
repeat(P, size_t, size_t) -> repeat<P, std::vector<right_of<P>>>;

template <class P>
concept char_parser = std::same_as<right_of<P>, char>;

// char -> std::string
template <char_parser P>
repeat(P, size_t, size_t) -> repeat<P, std::string>;

template <class P>
static inline auto many0(const P &p) {
    return repeat(p, 0);
}

template <char_parser P>
static inline auto many0(const P &p) {
    return repeat<P, std::string>(p, 0);
}

template <class P>
static inline auto many1(const P &p) {
    return repeat(p, 1);
}

template <char_parser P>
static inline auto many1(const P &p) {
    return repeat<P, std::string>(p, 1);
}

} // namespace tokenizes::repeats