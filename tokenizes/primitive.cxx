#pragma once
#include "primitive.hpp"
namespace tokenizes::primitive {

template <class T>
tag_mapper<T>::tag_mapper(const std::vector<std::tuple<std::string_view, T>> &m) {
    size_t size = 0;
    for (const auto &[key, _] : m) {
        for (ssize_t i = 0; i < key.size(); i++) {
            table.emplace(key.substr(0, i), std::nullopt);
        }
        size = std::max(size, key.size());
    }
    buffer_size = size;

    // convert
    for (const auto &[key, value] : m) {
        table.insert_or_assign(std::string(key), value);
    }
}

template <class T>
tag_mapper<T>::tag_mapper(std::initializer_list<std::tuple<std::string_view, T>> &&m) {
    size_t size = 0;
    for (const auto &[key, _] : m) {
        for (ssize_t i = 0; i < key.size(); i++) {
            table.emplace(key.substr(0, i), std::nullopt);
        }
        size = std::max(size, key.size());
    }
    buffer_size = size;

    // convert
    for (const auto &[key, value] : m) {
        table.insert_or_assign(std::string(key), value);
    }
}

template <class T>
either<T, std::nullptr_t> tag_mapper<T>::operator()(std::istream &is) const {

    std::string buffer;
    buffer.reserve(buffer_size);

    // rollback info
    T matched;
    ssize_t position = is.tellg();

    // non-matched loop
    do {
        const int input = is.get();
        if (input == -1) {
            // rollback
            is.seekg(position);
            return left(nullptr);
        }
        buffer.push_back(static_cast<char>(input));
        const auto iter = table.find(buffer);
        if (iter == table.end()) {
            // rollback
            is.seekg(position);
            return left(nullptr);
        }

        const auto &[_, value] = *iter;
        if (value) {
            // update rollback
            position = is.tellg();
            matched = *value;
            break;
        }

    } while (1);

    // matched loop
    do {
        const int input = is.get();
        if (input == -1) {
            // rollback
            is.seekg(position);
            return right(matched);
        }
        buffer.push_back(static_cast<char>(input));
        const auto iter = table.find(buffer);
        if (iter == table.end()) {
            // rollback
            is.seekg(position);
            return right(matched);
        }

        const auto &[_, value] = *iter;
        if (value) {
            // update rollback
            position = is.tellg();
            matched = *value;
        }
    } while (1);
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