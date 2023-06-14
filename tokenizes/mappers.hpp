#pragma once
#include "either.hpp"
#include <functional>
#include <istream>
#include <optional>
namespace tokenizes::mappers {

using tokenizes::eithers::either;
using tokenizes::eithers::either_mode;
using tokenizes::eithers::left;
using tokenizes::eithers::right;

template <class R1, class R2, class L>
class mapper {
public:
    using parser_t = std::function<either<R1, L>(std::istream &)>;
    using map_t = std::function<R2(R1)>;

private:
    parser_t parser;
    map_t map;

public:
    mapper(const parser_t &_parser, const map_t &_map) : parser(_parser), map(_map) {}
    mapper(parser_t &&_parser, map_t &&_map) : parser(_parser), map(_map) {}
    either<R2, L> operator()(std::istream &is) const {
        either<R1, L> result = parser(is);
        switch (result.get_mode()) {
        case either_mode::right:
            return right<R2>(map(result.get_right()));
        case either_mode::left:
            return left<L>(result.get_left());
        case either_mode::none:
            throw std::range_error("none cannot map");
        default:
            throw std::range_error("others case");
        }
    }
};

template <class F1, class F2>
mapper(F1, F2) -> mapper<typename std::invoke_result_t<F1, std::istream &>::right_t,
                         typename std::invoke_result_t<F2, typename std::invoke_result_t<F1, std::istream &>::right_t>,
                         typename std::invoke_result_t<F1, std::istream &>::left_t>;

// template <class R1, class R2>
// class opt_mapper {
// public:
//     using parser_t = std::function<std::optional<R1>(std::istream &)>;
//     using map_t = std::function<std::optional<R2>(R1)>;

// private:
//     parser_t parser;
//     map_t map;

// public:
//     opt_mapper(const parser_t &_parser, const map_t &_map) : parser(_parser), map(_map) {}
//     opt_mapper(parser_t &&_parser, map_t &&_map) : parser(_parser), map(_map) {}
//     std::optional<R2> operator()(std::istream &is) const {
//         std::optional<R1> result = parser(is);
//         if (!result) {
//             return std::nullopt;
//         }
//         return map(*result);
//     }
// };

// template <class F1, class F2>
// opt_mapper(F1, F2) -> opt_mapper<
//     typename std::invoke_result_t<F1, std::istream &>::value_type,
//     typename std::invoke_result_t<F2, typename std::invoke_result_t<F1, std::istream &>::value_type>::value_type>;

// template <class T, class V>
// class to_value {
// public:
//     using parser_t = std::function<std::optional<T>(std::istream &)>;

// private:
//     parser_t parser;
//     V value;

// public:
//     to_value(const parser_t &_parser, const V &_value) : parser(_parser), value(_value) {}
//     to_value(parser_t &&_parser, V &&_value) : parser(_parser), value(_value) {}
//     std::optional<V> operator()(std::istream &is) const {
//         std::optional<T> result = parser(is);
//         if (!result) {
//             return std::nullopt;
//         }
//         return value;
//     }
// };

// template <class F, class T>
// to_value(F, T) -> to_value<typename std::invoke_result_t<F, std::istream &>::value_type, T>;

} // namespace tokenizes::mappers