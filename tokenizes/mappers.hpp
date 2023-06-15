#pragma once
#include "concepts.hpp"
#include "either.hpp"
#include <concepts>
#include <functional>
#include <istream>
#include <optional>
namespace tokenizes::mappers {

using tokenizes::concepts::either_of;
using tokenizes::concepts::left_of;
using tokenizes::concepts::parsable;
using tokenizes::concepts::right_of;
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
        const either<R1, L> result = parser(is);
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

template <std::invocable<std::istream &> F1,
          std::invocable<typename std::invoke_result_t<F1, std::istream &>::right_t> F2>
mapper(F1, F2) -> mapper<typename std::invoke_result_t<F1, std::istream &>::right_t,
                         typename std::invoke_result_t<F2, typename std::invoke_result_t<F1, std::istream &>::right_t>,
                         typename std::invoke_result_t<F1, std::istream &>::left_t>;

template <concepts::parsable P, std::copy_constructible V>
class constant {

private:
    P parser;
    V value;

public:
    constant(const P &_parser, const V &_value) : parser(_parser), value(_value) {}
    constant(const constant &) = default;
    constant(constant &&) = default;

    either<V, left_of<P>> operator()(std::istream &is) const {
        either_of<P> result = parser(is);
        switch (result.get_mode()) {
        case either_mode::right:
            return right<V>(value);
        case either_mode::left:
            return left<left_of<P>>(result.get_left());
        case either_mode::none:
            throw std::range_error("none cannot map");
        default:
            throw std::range_error("others case");
        }
    }
};

} // namespace tokenizes::mappers