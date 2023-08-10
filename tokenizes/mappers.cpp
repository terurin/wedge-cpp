#include "mappers.hpp"
#include <iomanip>
namespace tokenizes::mappers {

std::ostream &operator<<(std::ostream &os, const position &p) {
    os << "range: (" << p.begin << "," << p.end << ")";
    return os;
}

} // namespace tokenizes::mappers