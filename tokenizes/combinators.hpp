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

template <parsable PX, parsable PY>
    requires std::same_as<left_of<PX>, left_of<PY>>
class sequencer {
    using X = right_of<PX>;
    using Y = right_of<PY>;
    using right_t = std::tuple<X, Y>;

    using left_t = left_of<PX>;
    using either_t = either<right_t, left_t>;

public:
    PX pr;
    PY pl;

public:
    sequencer(const PX &_pr, const PY &_pl) : pr(_pr), pl(_pl) {}
    sequencer(PX &&_pr, PY &&_pl) : pr(_pr), pl(_pl) {}

    either_t operator()(std::istream &is) const {

        // right
        either_of<PX> r = pr(is);
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
        either_of<PY> l = pl(is);
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

        return right<right_t>(std::make_tuple(std::move(r.get_right()), std::move(l.get_right())));
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