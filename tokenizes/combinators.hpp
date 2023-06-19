#pragma once
#include "concepts.hpp"
#include "either.hpp"
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace tokenizes::combinators {
using tokenizes::concepts::either_of;
using tokenizes::concepts::left_of;
using tokenizes::concepts::parsable;
using tokenizes::concepts::right_of;
using tokenizes::eithers::either;
using tokenizes::eithers::either_mode;
using tokenizes::eithers::left;
using tokenizes::eithers::right;

/** tuple version
 * X,Y  -> (X, Y)
 * Xs,Y -> (Xs,Y)
 * Xs,Ys-> (X, Ys)
 * Xs,Ys-> (Xs,Y)
 * */

template <class X, class Y>
std::tuple<X, Y> typed_merge(X &&x, Y &&y) {
    return std::make_tuple(std::move(x), std::move(y));
}

template <class... X, class Y>
std::tuple<X..., Y> typed_merge(std::tuple<X...> &&x, Y &&y) {
    return std::tuple_cat(std::move(x), std::make_tuple(y));
}

template <class X, class... Y>
std::tuple<X, Y...> typed_merge(X &&x, std::tuple<Y...> &&y) {
    return std::tuple_cat(std::make_tuple(x), std::move(y));
}

template <class... X, class... Y>
std::tuple<X..., Y...> typed_merge(std::tuple<X...> &&x, std::tuple<Y...> &&y) {
    return std::tuple_cat(std::move(x), std::move(y));
}

/** vector version
 * T,T -> [T]
 * [T],T -> [T]
 * T,[T] -> [T]
 * [T],[T] -> [T]
 */

template <class X, class Y>
    requires std::same_as<X, Y>
std::vector<X> typed_merge(X &&x, Y &&y) {
    return {x, y};
}

template <class X, class Y>
    requires std::same_as<X, Y>
std::vector<X> typed_merge(std::vector<X> &&x, Y &&y) {
    x.push_back(y);
    return x;
}

template <class X, class Y>
    requires std::same_as<X, Y>
std::vector<X> typed_merge(X &&x, std::vector<Y> &&y) {
    y.insert(y.begin(), x);
    return y;
}

template <class X, class Y>
    requires std::same_as<X, Y>
std::vector<X> typed_merge(std::vector<X> &&x, std::vector<Y> &&y) {
    x.insert(x.end(), y.begin(), y.end());
    return x;
}

/** string version
 * (char,  char)   -> string
 * (string,char)   -> string
 * (char,  string) -> string
 * (string,string) -> string
 */

static inline std::string typed_merge(char x, char y) { return {x, y}; }

static inline std::string typed_merge(std::string &&x, char y) {
    x.push_back(y);
    return x;
}

static inline std::string typed_merge(char x, std::string &&y) {
    y.insert(y.begin(), x);
    return y;
}

static inline std::string typed_merge(std::string &&x, std::string &&y) {
    x.insert(x.end(), y.begin(), y.end());
    return x;
}

/** nullptr version
 * (nullptr,T      ) -> T
 * (T,      nullptr) -> T
 * (nullptr,nullptr) -> nullptr
 */

template <class T>
static inline T typed_merge(T &&x, std::nullptr_t y) {
    (void)y;
    return x;
}

template <class T>
static inline T typed_merge(std::nullptr_t x, T &&y) {
    (void)x;
    return y;
}

static inline std::nullptr_t typed_merge(std::nullptr_t x, std::nullptr_t y) { return nullptr; }

template <parsable PX, parsable PY>
    requires std::same_as<left_of<PX>, left_of<PY>>
class sequencer {
public:
    using right_t = decltype([]() {
        right_of<PX> dx;
        right_of<PY> dy;
        return typed_merge(std::move(dx), std::move(dy));
    }());
    using left_t = left_of<PX>;
    using either_t = either<right_t, left_t>;

private:
    PX px;
    PY py;

public:
    sequencer(const PX &_pr, const PY &_pl) : px(_pr), py(_pl) {}
    sequencer(PX &&_pr, PY &&_pl) : px(_pr), py(_pl) {}

    either_t operator()(std::istream &is) const {
        // right
        either_of<PX> r = px(is);
        switch (r.get_mode()) {
        case either_mode::right:
            break;
        case either_mode::left:
            return r.into_left();
        case either_mode::none:
            throw std::range_error("none is unexpceted");
        default:
            throw std::range_error("others is unexpceted");
        }

        // left
        either_of<PY> l = py(is);
        switch (l.get_mode()) {
        case either_mode::right:
            break;
        case either_mode::left:
            return l.into_left();
        case either_mode::none:
            throw std::range_error("none is unexpceted");
        default:
            throw std::range_error("others is unexpceted");
        }

        return right(typed_merge(std::move(r.get_right()), std::move(l.get_right())));
    }
};

template <parsable PX, parsable PY>
    requires std::same_as<left_of<PX>, left_of<PY>>
auto operator*(const PX &r, const PY &l) {
    return sequencer<PX, PY>(r, l);
}

template <parsable PX, parsable PY>
    requires std::same_as<right_of<PX>, right_of<PY>>
class branch {
public:
    using right_t = right_of<PX>;
    using left_t = left_of<PY>;
    using either_t = either<right_t, left_t>;

private:
    PX px;
    PY py;

public:
    branch(const PX &_px, const PY &_py) : px(_px), py(_py) {}
    branch(PX &&_px, PY &&_py) : px(_px), py(_py) {}
    either_t operator()(std::istream &is) const {
        const std::streampos pos = is.tellg();
        {
            either_t e = px(is);
            switch (e.get_mode()) {
            case either_mode::right:
                return e.into_right();
            case either_mode::left:
                break;
            case either_mode::none:
                throw std::range_error("none is not support");
            default:
                throw std::domain_error("mode domain error");
            }
        }
        is.seekg(pos);
        {
            either_t e = px(is);
            switch (e.get_mode()) {
            case either_mode::right:
                return e.into_right();
            case either_mode::left:
                return e.into_left();
            case either_mode::none:
                throw std::range_error("none is not support");
            default:
                throw std::domain_error("mode domain error");
            }
        }
    }
};

template <parsable PX, parsable PY>
    requires std::same_as<right_of<PX>, right_of<PY>>
branch<PX, PY> operator+(const PX &px, const PY &py) {
    return branch(px, py);
}

} // namespace tokenizes::combinators