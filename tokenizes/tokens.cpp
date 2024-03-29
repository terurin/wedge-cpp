#include "tokens.hpp"
#include "combinators.hpp"
#include "parsers.hpp"
#include <algorithm>
#include <iomanip>
#include <unordered_map>
#include <vector>
namespace tokenizes::tokens {

using eithers::right, eithers::left;
using std::initializer_list;
using std::tuple, std::make_tuple;

struct general_record {
    token_id id;
    const char *name;
};

constinit const static general_record general_records[]{
#define member(x) {token_id::x, #x}
    member(variable), member(boolean), member(integer), member(real), member(text)
#undef member
};

struct mark_record {
    token_id id;
    const char *name;
    const char *mark;
};

constinit const static mark_record mark_records[]{
#define member(x, y) {token_id::x, #x, y}
    member(assign, "="), member(add, "+"), member(sub, "-"), member(mul, "*"), member(div, "*"), member(mod, "%"),
#undef member
};

std::ostream &operator<<(std::ostream &os, token_id id) {
    using std::string, std::optional, std::nullopt;

    const static auto find_id = [](token_id id) -> optional<const char *> {
        // general_records
        for (const auto &item : general_records) {
            if (item.id == id) {
                return item.name;
            }
        }

        // mark_records
        for (const auto &item : mark_records) {
            if (item.id == id) {
                return item.name;
            }
        }
        return nullopt;
    };

    const char *name = find_id(id).value_or("unknown");

    return os << name << "(0x" << std::hex << static_cast<int>(id) << ")";
}

std::ostream &operator<<(std::ostream &os, const value_t &v) {
    if (const bool *p = std::get_if<bool>(&v); p) {
        return os << (*p ? "true" : "false");
    }
    if (const int *p = std::get_if<int>(&v); p) {
        return os << *p;
    }
    if (const float *p = std::get_if<float>(&v); p) {
        return os << *p;
    }
    if (const std::string *p = std::get_if<std::string>(&v); p) {
        return os << std::quoted(*p);
    }
    return os << "none";
}

std::ostream &operator<<(std::ostream &os, const token &t) { return os << "id:" << t.id << ",value:" << t.value; }

token_parser::token_parser() {}

either<token, std::string> token_parser::operator()(std::istream &is) {

    const static auto marks =
        tokenizes::tag_mapper<token_id>([]() -> std::vector<std::tuple<std::string_view, token_id>> {
            std::vector<std::tuple<std::string_view, token_id>> table;
            table.reserve(sizeof(mark_records) / sizeof(mark_records[0]));
            for (const auto &item : mark_records) {
                table.push_back({item.mark, item.id});
            }
            return table;
        }())
            .positioned()
            .map_right([](const std::tuple<position, token_id> &args) {
                const auto &[pos, id] = args;
                return token(id, std::monostate(), pos);
            })
            .const_left(std::string("failed to parse marks"));

    const static auto integer = tokenizes::integer()
                                    .positioned()
                                    .map_right([](const std::tuple<position, int> &args) {
                                        const auto &[pos, value] = args;
                                        return token(token_id::integer, value, pos);
                                    })
                                    .const_left(std::string("failed to parse integer"));

    const static auto parser = combinators::branch(marks, integer);

    return parser(is);
}

} // namespace tokenizes::tokens
