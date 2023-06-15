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

template <parsable P, std::invocable<right_of<P>> M>
class mapper {
    using right_t = std::invoke_result_t<M, right_of<P>>;
    using left_t = left_of<P>;

private:
    P parser;
    M map;

public:
    mapper(const P &_parser, const M &_map) : parser(_parser), map(_map) {}
    either<right_t, left_t> operator()(std::istream &is) const {
        const either_of<P> result = parser(is);
        switch (result.get_mode()) {
        case either_mode::right:
            return right<right_t>(map(result.get_right()));
        case either_mode::left:
            return left<left_t>(result.get_left());
        case either_mode::none:
            throw std::range_error("none cannot map");
        default:
            throw std::range_error("others case");
        }
    }
};

template <parsable P, std::copy_constructible V>
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