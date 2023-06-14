#pragma once

#include "either.hpp"
#include <functional>
#include <istream>
#include <optional>
#include <vector>
namespace tokenizes::repeats {

using tokenizes::eithers::either;
using tokenizes::eithers::either_mode;
using tokenizes::eithers::left;
using tokenizes::eithers::right;
template <class R, class RC, class L>
class repeat {
public:
    using parser_t = std::function<either<R, L>(std::istream &)>;

private:
    parser_t parser;
    size_t n, m;

public:
    repeat(const parser_t &_parser, size_t _n = 0, size_t _m = SIZE_MAX) : parser(_parser), n(_n), m(_m) {}
    repeat(parser_t &&_parser, size_t _n = 0, size_t _m = SIZE_MAX) : parser(_parser), n(_n), m(_m) {}
    either<RC, L> operator()(std::istream &is) const {
        size_t i = 0;
        RC items;
        // head
        for (const auto head = is.tellg(); i < n; i++) {
            const either<R, L> item = parser(is);
            switch (item.get_mode()) {
            case either_mode::right:
                items.push_back(item.get_right());
                break;
            case either_mode::left:
                is.seekg(head);
                return left<L>(item.get_left());
            case either_mode::none:
                throw std::range_error("none is unexpceted");
            default:
                throw std::range_error("others is unexpceted");
            }
        }

        // tail
        for (; i < m; i++) {
            const auto tail = is.tellg();
            const either<R, L> item = parser(is);
            if (!item.is_right()) {
                is.seekg(tail);
                return right<RC>(items);
            }
            items.push_back(item.get_right());
        }
        return right<RC>(items);
    }
};

// T -> vector<T>
template <class P>
repeat(P, size_t, size_t) -> repeat<typename std::invoke_result_t<P, std::istream &>::right_t,
                                    std::vector<typename std::invoke_result_t<P, std::istream &>::right_t>,
                                    typename std::invoke_result_t<P, std::istream &>::left_t>;

template <class P>
concept char_parser = std::same_as<typename std::invoke_result_t<P, std::istream &>::right_t, char>;

// char -> std::string
template <char_parser P>
repeat(P, size_t, size_t) -> repeat<typename std::invoke_result_t<P, std::istream &>::right_t, std::string,
                                    typename std::invoke_result_t<P, std::istream &>::left_t>;

template <class P>
static inline auto many0(const P &p) {
    using R = std::invoke_result_t<P, std::istream &>::right_t;
    using L = std::invoke_result_t<P, std::istream &>::left_t;
    return repeat<R, std::vector<R>, L>(p, 0);
}

template <class P>
static inline auto many0(P &&p) {
    using R = std::invoke_result_t<P, std::istream &>::right_t;
    using L = std::invoke_result_t<P, std::istream &>::left_t;
    return repeat<R, std::vector<R>, L>(std::move(p), 0);
}

template <char_parser P>
static inline auto many0(const P &p) {
    using L = std::invoke_result_t<P, std::istream &>::left_t;
    return repeat<char, std::string, L>(p, 0);
}

template <char_parser P>
static inline auto many0(P &&p) {
    using L = std::invoke_result_t<P, std::istream &>::left_t;
    return repeat<char, std::string, L>(std::move(p), 0);
}

template <class P>
static inline auto many1(const P &p) {
    using R = std::invoke_result_t<P, std::istream &>::right_t;
    using L = std::invoke_result_t<P, std::istream &>::left_t;
    return repeat<R, std::vector<R>, L>(p, 1);
}

template <class P>
static inline auto many1(P &&p) {
    using R = std::invoke_result_t<P, std::istream &>::right_t;
    using L = std::invoke_result_t<P, std::istream &>::left_t;
    return repeat<R, std::vector<R>, L>(std::move(p), 1);
}

template <char_parser P>
static inline auto many1(const P &p) {
    using L = std::invoke_result_t<P, std::istream &>::left_t;
    return repeat<char, std::string, L>(p, 1);
}

template <char_parser P>
static inline auto many1(P &&p) {
    using L = std::invoke_result_t<P, std::istream &>::left_t;
    return repeat<char, std::string, L>(std::move(p), 1);
}

} // namespace tokenizes::repeats