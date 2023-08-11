#pragma once
#include "primitive.hpp"
namespace tokenizes::primitive {

template <class T>
template <std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, std::tuple<std::string_view, T>>
constexpr tag_mapper<T>::node_ptr tag_mapper<T>::build_root(R &&r) {

    node_ptr root = std::make_unique<node_t>();
    for (const auto &[key, value] : r) {
        build_node(*root, key, value);
    }
    return root;
}

template <class T>
constexpr void tag_mapper<T>::build_node(node_t &node, std::string_view key, const T &value) {
    if (key.size() == 0) {
        node.value = value; // terminal
        return;
    }

    const size_t index = static_cast<size_t>(key[0]);
    const std::string_view next_key = key.substr(1);
    assert(index < 256);

    if (!node.table[index]) {
        node_ptr next_node = std::make_unique<node_t>();
        build_node(*next_node, next_key, value);
        node.table[index] = std::move(next_node);
    } else {
        build_node(*node.table[index], next_key, value);
    }
}

template <class T>
constexpr std::optional<T> tag_mapper<T>::walk_node(const node_t &node, std::istream &is) {

    const int index = is.get();

    if (index == -1) {
        return node.value;
    }
    assert(0 <= index && index < 256);

    if (node.table[index]) {
        const node_t &next_node = *node.table[index];
        const std::streampos pos = is.tellg();
        if (const std::optional<T> next_value = walk_node(next_node, is); next_value) {
            return next_value;
        }
        is.seekg(pos);
    }

    return node.value;
}

template <std::unsigned_integral T>
either<T, unsigned_errors> unsigned_parser<T>::operator()(std::istream &is) const {
    using namespace std;
    const std::streampos pos = is.tellg();
    T result = 0;

    // first
    if (const either<int, nullptr_t> e = digit(is); e.is_right()) {
        result = e.get_right();
    } else {
        return left(unsigned_errors::not_digit);
    }

    // lasts
    const unsigned int base = get_base();
    for (either<int, nullptr_t> e = digit(is); e.is_right(); e = digit(is)) {
        const T limit = std::numeric_limits<T>::max() - result;
        const int d = e.get_right();

        // shift
        if (result * (base - 1) + d > limit) {
            is.seekg(pos);
            return left(unsigned_errors::overflow);
        }
        result = result * base + d;
    }
    return right(result);
}

template <std::signed_integral T>
either<T, signed_errors> signed_parser<T>::operator()(std::istream &is) const {
    const std::streampos pos = is.tellg();

    // [-+]?
    bool sign = false;
    if (const int s = is.peek(); s == '+' || s == '-') {
        sign = s == '-';
        is.ignore();
    }

    //[0-(base-1)]
    T result = 0;
    if (const either<int, std::nullptr_t> e = digit(is); e.is_right()) {
        result = sign ? -e.get_right() : e.get_right();
    } else {
        return left(signed_errors::not_digit);
    }

    //[0-(base-1)]*
    const unsigned int base = digit.get_base();
    for (either<int, std::nullptr_t> e = digit(is); e.is_right(); e = digit(is)) {
        const int d = e.get_right();
        if (sign) {
            const T limit = std::numeric_limits<T>::min() - result;
            if (result * (base - 1) - d < limit) {
                is.seekg(pos);
                return left(signed_errors::underflow);
            }
            result = result * base - d;

        } else {
            const T limit = std::numeric_limits<T>::max() - result;
            if (result * (base - 1) + d > limit) {
                is.seekg(pos);
                return left(signed_errors::overflow);
            }
            result = result * base + d;
        }
    }

    return right(result);
}

template <std::signed_integral T>
either<T, integer_errors> integer_parser<T>::operator()(std::istream &is) const {
    const std::streampos pos = is.tellg();
    // [+-]?
    bool sign = false;
    if (const int s = is.peek(); s == '+' || s == '-') {
        sign = s == '-';
        is.ignore();
    }

    // attempt {0b,0q,0o,0d,0x}?
    const int base = [](std::istream &is) -> int {
        const std::streampos pos = is.tellg();
        if (is.peek() != '0') {
            return 10;
        }
        is.ignore();
        switch (is.get()) {
        case 'b':
            return 2;
        case 'q':
            return 4;
        case 'o':
            return 8;
        case 'd':
            return 10;
        case 'x':
            return 16;
        default:
            is.seekg(pos);
            return 10;
        }
    }(is);
    const digit_parser digit(base);

    // firts
    T result = 0;

    if (const either<int, std::nullptr_t> e = digit(is); e.is_right()) {
        result = sign ? -e.get_right() : e.get_right();
    } else {
        return left(integer_errors::not_digit);
    }

    //[0-(base-1)]*
    for (either<int, std::nullptr_t> e = digit(is); e.is_right(); e = digit(is)) {
        const int d = e.get_right();
        if (sign) {
            const T limit = std::numeric_limits<T>::min() - result;
            if (result * (base - 1) - d < limit) {
                is.seekg(pos);
                return left(integer_errors::underflow);
            }
            result = result * base - d;

        } else {
            const T limit = std::numeric_limits<T>::max() - result;
            if (result * (base - 1) + d > limit) {
                is.seekg(pos);
                return left(integer_errors::overflow);
            }
            result = result * base + d;
        }
    }

    return right(result);
}

} // namespace tokenizes::primitive