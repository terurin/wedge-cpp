#pragma once
#include "concepts.hpp"
#include "either.hpp"
#include <stdexcept>
#include <tuple>
namespace tokenizes::combinators {
using tokenizes::concepts::either_of;
using tokenizes::concepts::left_of;
using tokenizes::concepts::parsable;
using tokenizes::concepts::right_of;
using tokenizes::eithers::either;
using tokenizes::eithers::either_mode;
using tokenizes::eithers::left;
using tokenizes::eithers::right;

template <class X, class Y>
std::tuple<X, Y> tupled_merge(X x, Y y) {
    return std::make_tuple(x, y);
}

template <class... X, class Y>
std::tuple<X..., Y> tupled_merge(std::tuple<X...> x, Y y) {
    return std::tuple_cat(x, std::make_tuple(y));
}

template <class X, class... Y>
std::tuple<X, Y...> tupled_merge(X x, std::tuple<Y...> y) {
    return std::tuple_cat(std::make_tuple(x), y);
}

template <class... X, class... Y>
std::tuple<X..., Y...> tupled_merge(std::tuple<X...> x, std::tuple<Y...> y) {
    return std::tuple_cat(x, y);
}

template <parsable PX, parsable PY>
    requires std::same_as<left_of<PX>, left_of<PY>>
class sequencer {
    using X = right_of<PX>;
    using Y = right_of<PY>;

public:
    PX px;
    PY py;

    using right_t = decltype([]() {
        right_of<PX> dx;
        right_of<PY> dy;
        return tupled_merge(std::move(dx), std::move(dy));
    }());
    using left_t = left_of<PX>;
    using either_t = either<right_t, left_t>;

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

        return right(tupled_merge(std::move(r.get_right()), std::move(l.get_right())));
    }
};

template <parsable PX, parsable PY>
    requires std::same_as<left_of<PX>, left_of<PY>>
auto operator*(const PX &r, const PY &l) {
    return sequencer<PX, PY>(r, l);
}

template <parsable PX, parsable PY>
    requires std::same_as<left_of<PX>, left_of<PY>>
auto operator*(PX &&r, PY &&l) {
    return sequencer<PX, PY>(std::move(r), std::move(l));
}

} // namespace tokenizes::combinators