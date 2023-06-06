#pragma once
#include <functional>
#include <istream>
#include <optional>

namespace tokenizes::mappers {

template <class T1, class T2>
class mapper {
public:
    using parser_t = std::function<std::optional<T1>(std::istream &)>;
    using map_t = std::function<T2(T1)>;

private:
    parser_t parser;
    map_t map;

public:
    mapper(const parser_t &_parser, const map_t &_map) : parser(_parser), map(_map) {}
    mapper(parser_t &&_parser, map_t &&_map) : parser(_parser), map(_map) {}
    std::optional<T2> operator()(std::istream &is) const {
        std::optional<T1> result = parser(is);
        if (!result) {
            return std::nullopt;
        }
        return map(*result);
    }
};

template <class F1, class F2>
mapper(F1, F2)
    -> mapper<typename std::invoke_result_t<F1, std::istream &>::value_type,
              typename std::invoke_result_t<F2, typename std::invoke_result_t<F1, std::istream &>::value_type>>;

template <class T1, class T2>
class opt_mapper {
public:
    using parser_t = std::function<std::optional<T1>(std::istream &)>;
    using map_t = std::function<std::optional<T2>(T1)>;

private:
    parser_t parser;
    map_t map;

public:
    opt_mapper(const parser_t &_parser, const map_t &_map) : parser(_parser), map(_map) {}
    opt_mapper(parser_t &&_parser, map_t &&_map) : parser(_parser), map(_map) {}
    std::optional<T2> operator()(std::istream &is) const {
        std::optional<T1> result = parser(is);
        if (!result) {
            return std::nullopt;
        }
        return map(*result);
    }
};

template <class F1, class F2>
opt_mapper(F1, F2) -> opt_mapper<
    typename std::invoke_result_t<F1, std::istream &>::value_type,
    typename std::invoke_result_t<F2, typename std::invoke_result_t<F1, std::istream &>::value_type>::value_type>;

template <class T, class V>
class to_value {
public:
    using parser_t = std::function<std::optional<T>(std::istream &)>;

private:
    parser_t parser;
    V value;

public:
    to_value(const parser_t &_parser, const V &_value) : parser(_parser), value(_value) {}
    to_value(parser_t &&_parser, V &&_value) : parser(_parser), value(_value) {}
    std::optional<V> operator()(std::istream &is) const {
        std::optional<T> result = parser(is);
        if (!result) {
            return std::nullopt;
        }
        return value;
    }
};

template <class F, class T>
to_value(F, T) -> to_value<typename std::invoke_result_t<F, std::istream &>::value_type, T>;

} // namespace tokenizes::mappers