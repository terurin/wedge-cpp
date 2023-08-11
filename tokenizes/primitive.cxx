#pragma once
#include "primitive.hpp"
namespace tokenizes::primitive {

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