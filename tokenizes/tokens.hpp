#pragma once
#include "either.hpp"
#include "mappers.hpp"
#include "primitive.hpp"
#include <ios>
#include <memory>
#include <string>
#include <variant>
namespace tokenizes::tokens {

using eithers::either;
using mappers::position;

constexpr static inline uint32_t token_id_specials = 0 << 16;
constexpr static inline uint32_t token_id_marks = 1 << 16;

enum class token_id : uint32_t {
    // literals
    variable = token_id_specials,
    boolean,
    integer,
    real,
    text,
    // marks
    assign = token_id_marks,
    add,
    sub,
    mul,
    div,
    mod,
};

std::ostream &operator<<(std::ostream &, token_id);

constexpr static inline bool is_mark(token_id id) {
    const uint32_t value = static_cast<uint32_t>(id);
    return (value & token_id_marks) == token_id_marks;
}

using value_t = std::variant<std::monostate, bool, int, float, std::string>;

std::ostream &operator<<(std::ostream &, const value_t &);

struct token {
    const token_id id;
    const value_t value;
    const position pos;

    token(token_id _id, const value_t &_value, const position &_pos) : id(_id), value(_value), pos(_pos) {}
    token(token_id _id, value_t &&_value, position &&_pos) : id(_id), value(_value), pos(_pos) {}
};

std::ostream &operator<<(std::ostream &, const token &);

class token_parser {
public:
    using mark_parser = mappers::positioned<mappers::tag_mapper<token_id>>;

private:
    const static mark_parser marks;

public:
    token_parser();
    either<token, std::string> operator()(std::istream &is);
};

} // namespace tokenizes::tokens