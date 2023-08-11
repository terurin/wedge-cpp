#pragma once
#include "concepts.hpp"
#include "either.hpp"
#include <concepts>
#include <cstddef>
#include <exception>
#include <functional>
#include <ios>
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
class mapper_right {
    using right_t = std::invoke_result_t<M, right_of<P>>;
    using left_t = left_of<P>;

private:
    P parser;
    M map;

public:
    mapper_right(const P &_parser, M &&_map) : parser(_parser), map(_map) {}
    mapper_right(P &&_parser, M &&_map) : parser(_parser), map(_map) {}
    either<right_t, left_t> operator()(std::istream &is) const {
        either_of<P> result = parser(is);
        switch (result.get_mode()) {
        case either_mode::right:
            return right<right_t>(map(result.get_right()));
        case either_mode::left:
            return result.into_left();
        case either_mode::none:
            throw std::range_error("none cannot map");
        default:
            throw std::domain_error("mode domain error");
        }
    }
};

template <parsable P, std::invocable<left_of<P>> M>
class mapper_left {
    using right_t = right_of<P>;
    using left_t = std::invoke_result_t<M, left_of<P>>;

private:
    P parser;
    M map;

public:
    mapper_left(const P &_parser, M &&_map) : parser(_parser), map(_map) {}
    mapper_left(P &&_parser, M &&_map) : parser(_parser), map(_map) {}

    either<right_t, left_t> operator()(std::istream &is) const {
        either_of<P> result = parser(is);
        switch (result.get_mode()) {
        case either_mode::right:
            return result.into_right();
        case either_mode::left:
            return left<left_t>(map(result.get_left()));
        case either_mode::none:
            throw std::range_error("none cannot map");
        default:
            throw std::domain_error("mode domain error");
        }
    }
};

template <parsable P, class V>
    requires std::move_constructible<V> && std::copy_constructible<V>
class constant_right {

private:
    P parser;
    V value;

public:
    constant_right(const P &_parser, V &&_value) : parser(_parser), value(_value) {}
    constant_right(P &&_parser, V &&_value) : parser(_parser), value(_value) {}

    either<V, left_of<P>> operator()(std::istream &is) const {
        either_of<P> result = parser(is);
        switch (result.get_mode()) {
        case either_mode::right:
            return right<V>(value);
        case either_mode::left:
            return result.into_left();
        case either_mode::none:
            throw std::range_error("none cannot map");
        default:
            throw std::domain_error("mode domain error");
        }
    }
};

template <parsable P, class V>
    requires std::move_constructible<V> && std::copy_constructible<V>
class constant_left {

private:
    P parser;
    V value;

public:
    constant_left(const P &_parser, V &&_value) : parser(_parser), value(_value) {}
    constant_left(P &&_parser, V &&_value) : parser(_parser), value(_value) {}

    either<right_of<P>, V> operator()(std::istream &is) const {
        either_of<P> result = parser(is);
        switch (result.get_mode()) {
        case either_mode::right:
            return result.into_right();
        case either_mode::left:
            return left<V>(value);
        case either_mode::none:
            throw std::range_error("none cannot map");
        default:
            throw std::domain_error("mode domain error");
        }
    }
};

template <parsable P>
class eraser_right {
    P parser;

public:
    eraser_right(const P &_parser) : parser(_parser) {}
    eraser_right(P &&_parser) : parser(_parser) {}
    either<std::nullptr_t, left_of<P>> operator()(std::istream &is) const {
        either_of<P> result = parser(is);
        switch (result.get_mode()) {
        case either_mode::right:
            return right(nullptr);
        case either_mode::left:
            return result.into_left();
        case either_mode::none:
            throw std::range_error("none cannot map");
        default:
            throw std::domain_error("mode domain error");
        }
    }
};

template <parsable P>
class eraser_left {
    P parser;

public:
    eraser_left(const P &_parser) : parser(_parser) {}
    eraser_left(P &&_parser) : parser(_parser) {}
    either<right_of<P>, std::nullptr_t> operator()(std::istream &is) const {
        either_of<P> result = parser(is);
        switch (result.get_mode()) {
        case either_mode::right:
            return result.into_right();
        case either_mode::left:
            return left(nullptr);
        case either_mode::none:
            throw std::range_error("none cannot map");
        default:
            throw std::domain_error("mode domain error");
        }
    }
};

template <parsable P>
class eraser_both {
    P parser;

public:
    eraser_both(const P &_parser) : parser(_parser) {}
    eraser_both(P &&_parser) : parser(_parser) {}
    either<std::nullptr_t, std::nullptr_t> operator()(std::istream &is) const {
        either_of<P> result = parser(is);
        switch (result.get_mode()) {
        case either_mode::right:
            return right(nullptr);
        case either_mode::left:
            return left(nullptr);
        case either_mode::none:
            throw std::range_error("none cannot map");
        default:
            throw std::domain_error("mode domain error");
        }
    }
};

template <parsable P>
class recognition {
    P parser;

public:
    recognition(const P &_parser) : parser(_parser) {}
    recognition(P &&_parser) : parser(_parser) {}
    either<std::string, left_of<P>> operator()(std::istream &is) const {
        const std::streampos begin = is.tellg();
        either_of<P> result = parser(is);
        const std::streampos end = is.tellg();

        switch (result.get_mode()) {
        case either_mode::right: {
            std::string buffer;
            buffer.reserve(end - begin);

            is.seekg(begin);
            for (auto pos = is.tellg(); pos != end; pos = is.tellg()) {
                buffer.push_back(is.get());
            }

            return right<std::string>(buffer);
        }
        case either_mode::left:
            return result.into_left();
        case either_mode::none:
            throw std::range_error("none cannot map");
        default:
            throw std::domain_error("mode domain error");
        }
    }
};

struct position {
    const std::streampos begin, end;
    position(std::streampos _begin, std::streampos _end) : begin(_begin), end(_end) {}
    size_t size() const { return end - begin; }
};

std::ostream &operator<<(std::ostream &os, const position &p);

template <parsable P>
class positioned {
public:
    using right_t = std::tuple<position, right_of<P>>;
    using left_t = left_of<P>;

private:
    P parser;

public:
    constexpr positioned(const P &_parser) : parser(_parser) {}
    constexpr positioned(P &&_parser) : parser(_parser) {}
    either<right_t, left_t> operator()(std::istream &is) const {
        const std::streampos begin = is.tellg();
        const either<right_of<P>, left_of<P>> result = parser(is);

        switch (result.get_mode()) {
        case either_mode::right: {
            const std::streampos end = is.tellg();
            return right(std::make_tuple<position, right_of<P>>(position(begin, end), result.get_right()));
        }

        case either_mode::left:
            return result.into_left();
        case either_mode::none:
            throw std::range_error("result is none");
        default:
            throw std::domain_error("mode domain error");
        }
    }
};

} // namespace tokenizes::mappers