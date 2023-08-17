#pragma once
#include "concepts.hpp"
#include "either.hpp"
#include <cassert>
#include <concepts>
#include <cstddef>
#include <exception>
#include <functional>
#include <ios>
#include <istream>
#include <memory>
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
    mapper_right(const P &_parser, M &&_map)
        requires std::copy_constructible<P> && std::move_constructible<M>
        : parser(_parser), map(_map) {}
    mapper_right(P &&_parser, M &&_map)
        requires std::move_constructible<P> && std::move_constructible<M>
        : parser(_parser), map(_map) {}
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
    mapper_left(const P &_parser, M &&_map)
        requires std::copy_constructible<P> && std::move_constructible<M>
        : parser(_parser), map(_map) {}
    mapper_left(P &&_parser, M &&_map)
        requires std::move_constructible<P> && std::move_constructible<M>
        : parser(_parser), map(_map) {}

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
    constant_right(const P &_parser, V &&_value)
        requires std::copy_constructible<P> && std::move_constructible<V>
        : parser(_parser), value(_value) {}
    constant_right(P &&_parser, V &&_value)
        requires std::move_constructible<P> && std::move_constructible<V>
        : parser(_parser), value(_value) {}

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
    constant_left(const P &_parser, V &&_value)
        requires std::copy_constructible<P> && std::move_constructible<V>
        : parser(_parser), value(_value) {}
    constant_left(P &&_parser, V &&_value)
        requires std::move_constructible<P> && std::move_constructible<V>
        : parser(_parser), value(_value) {}

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
    eraser_right(const P &_parser)
        requires std::copy_constructible<P>
        : parser(_parser) {}
    eraser_right(P &&_parser)
        requires std::move_constructible<P>
        : parser(_parser) {}
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
    eraser_left(const P &_parser)
        requires std::copy_constructible<P>
        : parser(_parser) {}
    eraser_left(P &&_parser)
        requires std::move_constructible<P>
        : parser(_parser) {}
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
    eraser_both(const P &_parser)
        requires std::copy_constructible<P>
        : parser(_parser) {}
    eraser_both(P &&_parser)
        requires std::move_constructible<P>
        : parser(_parser) {}
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
    recognition(const P &_parser)
        requires std::copy_constructible<P>
        : parser(_parser) {}
    recognition(P &&_parser)
        requires std::move_constructible<P>
        : parser(_parser) {}
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

template <class T>
class tag_mapper {

public:
    class node {
        std::optional<T> value{std::nullopt};
        std::array<std::unique_ptr<node>, 256> table;

    public:
        node() { std::ranges::fill(table, nullptr); }

        template <std::ranges::input_range R>
            requires std::convertible_to<std::ranges::range_value_t<R>, std::tuple<std::string_view, T>>
        node(R &&range) {
            std::ranges::fill(table, nullptr);
            for (const auto &[key, value] : range) {
                insert(key, value);
            }
        }

        std::optional<T> find(std::istream &is) const {
            const int index = is.get();

            if (index == -1) {
                return value;
            }
            assert(0 <= index && index < 256);

            if (table[index]) {
                const node &next_node = *table[index];
                const std::streampos pos = is.tellg();
                if (const std::optional<T> next_value = next_node.find(is); next_value) {
                    return next_value;
                }
                is.seekg(pos);
            }

            return value;
        }

        void insert(std::string_view key, const T &value) {
            if (key.size() == 0) {
                this->value = value; // terminal
                return;
            }

            const size_t index = static_cast<size_t>(key[0]);
            const std::string_view next_key = key.substr(1);
            assert(index < 256);

            if (!table[index]) {
                auto next_node = std::make_unique<node>();
                next_node->insert(next_key, value);
                table[index] = std::move(next_node);
            } else {
                table[index]->insert(next_key, value);
            }
        }
    };

private:
    mutable std::shared_ptr<node> root;

public:
    template <std::ranges::input_range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, std::tuple<std::string_view, T>>
    tag_mapper(R &&r) : root(std::make_shared<node>(r)) {}
    tag_mapper(std::initializer_list<std::tuple<std::string_view, T>> &&list) : root(std::make_shared<node>(list)) { ; }
    tag_mapper(const tag_mapper &tm) : root(tm.root) {}
    either<T, std::nullptr_t> operator()(std::istream &is) const {
        if (const std::optional<T> opt = root->find(is); opt) {
            return right(*opt);
        }
        return left(nullptr);
    }
};

struct position {
    const size_t begin, end;
    constexpr position(size_t _begin, size_t _end) : begin(_begin), end(_end) {}
    constexpr size_t size() const { return end - begin; }
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
    constexpr positioned(const P &_parser)
        requires std::copy_constructible<P>
        : parser(_parser) {}
    constexpr positioned(P &&_parser)
        requires std::move_constructible<P>
        : parser(std::move(_parser)) {}
    either<right_t, left_t> operator()(std::istream &is) const {
        const std::streampos begin = is.tellg();
        either<right_of<P>, left_of<P>> result = parser(is);

        switch (result.get_mode()) {
        case either_mode::right: {
            const std::streampos end = is.tellg();

            return right(std::tuple<position, right_of<P>>(position(begin, end), result.get_right()));
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

#include "mappers.cxx"
