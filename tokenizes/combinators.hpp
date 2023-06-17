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

template <parsable PR, parsable PL>
    requires std::same_as<left_of<PR>, left_of<PL>>
class sequencer {
    using right_t = std::tuple<right_of<PR>, right_of<PL>>;
    using left_t = left_of<PR>;
    using either_t = either<right_t, left_t>;

public:
    PR pr;
    PL pl;

public:
    sequencer(const PR &_pr, const PL &_pl) : pr(_pr), pl(_pl) {}
    sequencer(PR &&_pr, PL &&_pl) : pr(_pr), pl(_pl) {}

    either_t operator()(std::istream &is) const {

        // right
        either_of<PR> pr_result = pr(is);
        switch (pr_result.get_mode()) {
        case either_mode::right:
            break;
        case either_mode::left:
            return pr_result.into_left();
        case either_mode::none:
            throw std::range_error("none is unexpceted");
        default:
            throw std::range_error("others is unexpceted");
        }

        // left
        either_of<PL> pl_result = pl(is);
        switch (pl_result.get_mode()) {
        case either_mode::right:
            break;
        case either_mode::left:
            return pl_result.into_left();
        case either_mode::none:
            throw std::range_error("none is unexpceted");
        default:
            throw std::range_error("others is unexpceted");
        }

        return right<right_t>(std::make_tuple(std::move(pr_result.get_right()), std::move(pl_result.get_right())));
    }
};
} // namespace tokenizes::combinators