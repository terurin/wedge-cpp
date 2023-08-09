#pragma once
#include "either.hpp"
#include "primitive.hpp"
#include <ios>
#include <memory>
#include <string>
#include <variant>
namespace tokenizes::tokens {

using eithers::either;

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
    token_id id;
    value_t value;
    // size_t begin, end; position

    token_id get_id() const { return id; }
    const value_t &get_value() const { return value; }
};

std::ostream &operator<<(std::ostream &, const token &);

class token_parser {
    const static primitive::tag_mapper<token_id> marks;

public:
    token_parser();
    either<token, std::nullptr_t> operator()(std::istream &is);
};

} // namespace tokenizes::tokens