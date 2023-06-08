#pragma once

#include <functional>
#include <istream>
#include <optional>
#include <vector>
namespace tokenizes::repeats {

template <class T, class C>
class repeat {
public:
    using parser_t = std::function<std::optional<T>(std::istream &)>;

private:
    parser_t parser;
    size_t n, m;

public:
    repeat(const parser_t &_parser, size_t _n = 0, size_t _m = SIZE_MAX) : parser(_parser), n(_n), m(_m) {}
    repeat(parser_t &&_parser, size_t _n = 0, size_t _m = SIZE_MAX) : parser(_parser), n(_n), m(_m) {}
    std::optional<C> operator()(std::istream &is) const {
        size_t i = 0;
        C items;
        // head
        for (const auto head = is.tellg(); i < n; i++) {
            const std::optional<T> item = parser(is);
            if (!item) {
                is.seekg(head);
                return std::nullopt;
            }
            items.push_back(*item);
        }

        // tail
        for (; i < m; i++) {
            const auto tail = is.tellg();
            const std::optional<T> item = parser(is);
            if (!item) {
                is.seekg(tail);
                return items;
            }
            items.push_back(*item);
        }
        return items;
    }
};

// T -> vector<T>
template <class P>
repeat(P, size_t, size_t) -> repeat<typename std::invoke_result_t<P, std::istream &>::value_type,
                                    std::vector<typename std::invoke_result_t<P, std::istream &>::value_type>>;

template <class P>
concept char_parser = std::same_as<typename std::invoke_result_t<P, std::istream &>::value_type, char>;

// char -> std::string
template <char_parser P>
repeat(P, size_t, size_t) -> repeat<typename std::invoke_result_t<P, std::istream &>::value_type, std::string>;

template <class P>
static inline auto many0(const P &p) {
    using T = std::invoke_result_t<P, std::istream &>::value_type;
    return repeat<T, std::vector<T>>(p, 0);
}

template <class P>
static inline auto many0(P &&p) {
    using T = std::invoke_result_t<P, std::istream &>::value_type;
    return repeat<T, std::vector<T>>(std::move(p), 0);
}

template <char_parser P>
static inline auto many0(const P &p) {
    return repeat<char, std::string>(p, 0);
}

template <char_parser P>
static inline auto many0(P &&p) {
    using T = std::invoke_result_t<P, std::istream &>::value_type;
    return repeat<char, std::string>(std::move(p), 0);
}

template <class P>
static inline auto many1(const P &p) {
    using T = std::invoke_result_t<P, std::istream &>::value_type;
    return repeat<T, std::vector<T>>(p, 1);
}

template <class P>
static inline auto many1(P &&p) {
    using T = std::invoke_result_t<P, std::istream &>::value_type;
    return repeat<T, std::vector<T>>(std::move(p), 1);
}

template <char_parser P>
static inline auto many1(const P &p) {
    return repeat<char, std::string>(p, 1);
}

template <char_parser P>
static inline auto many1(P &&p) {
    using T = std::invoke_result_t<P, std::istream &>::value_type;
    return repeat<char, std::string>(std::move(p), 1);
}

template <class P>
static inline auto some(const P &p) {
    using T = std::invoke_result_t<P, std::istream &>::value_type;
    return repeat<T, std::vector<T>>(p, 0, 1);
}

template <class P>
static inline auto some(P &&p) {
    using T = std::invoke_result_t<P, std::istream &>::value_type;
    return repeat<T, std::vector<T>>(std::move(p), 0, 1);
}

template <char_parser P>
static inline auto some(const P &p) {
    return repeat<char, std::string>(p, 0, 1);
}

template <char_parser P>
static inline auto some(P &&p) {
    using T = std::invoke_result_t<P, std::istream &>::value_type;
    return repeat<char, std::string>(std::move(p), 0, 1);
}

} // namespace tokenizes::repeats