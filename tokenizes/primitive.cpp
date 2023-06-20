#include "primitive.hpp"
#include <algorithm>
#include <iomanip>
#include <map>
namespace tokenizes::primitive {

using tokenizes::eithers::left;
using tokenizes::eithers::right;

struct escaped_char {
    char c;
    escaped_char(char _c) : c(_c) {}
};

static std::ostream &operator<<(std::ostream &os, const escaped_char &ec) {
    const char c = ec.c;

    if (isgraph(c)) {
        char buffer[16];
        snprintf(buffer, 256, "'%c'", c);
        return os << buffer;
    }

    if (!isspace(c)) {
        char buffer[16];
        snprintf(buffer, 256, "%02x", c);
        return os << buffer;
    }

    switch (c) {
    case ' ':
        return os << "' '";
    case '\f':
        return os << "\\f";
    case '\n':
        return os << "\\n";
    case '\r':
        return os << "\\n";
    case '\t':
        return os << "\\t";
    case '\v':
        return os << "\\v";
    default:
        return os;
    }
}

std::ostream &operator<<(std::ostream &os, const atom &s) {
    const auto chars = s.get_chars();

    if (chars.none()) {
        return os << "{}";
    }
    if (chars.all()) {
        return os << "{all}";
    }

    os << "{";
    bool use_comma = false;
    for (size_t i = 0; i < 256; i++) {
        if (!chars.test(i)) continue;

        if (use_comma) {
            os << "," << escaped_char((char)i);
        } else {
            os << escaped_char((char)i);
        }
        use_comma = true;
    }
    os << "}";
    return os;
}

either<char, std::nullptr_t> atom::operator()(std::istream &ss) const {
    std::string r;

    const int input = ss.peek();
    if (input == -1 || !chars.test(input)) {
        return left(nullptr);
    }
    ss.ignore();
    return right(input);
}

either<std::string, std::nullptr_t> tag::operator()(std::istream &ss) const {
    std::string r;
    auto pos = ss.tellg();
    for (const char c : str) {
        const int input = ss.get();
        if (input != (int)c) {
            ss.seekg(pos);
            return left(nullptr);
        }
    }
    return right(str);
}

std::ostream &operator<<(std::ostream &os, const tag &t) {
    os << "tag: " << std::quoted(t.get_str());

    return os;
}

tag_list::tag_list(const std::vector<std::string> &list) {
    for (const std::string &item : list) {
        for (ssize_t i = 0; i < item.size(); i++) {
            table.emplace(item.substr(0, i), false);
        }
    }

    size_t size = 0;
    for (const std::string &item : list) {
        table.insert_or_assign(item, true);
        size = std::max(size, item.size());
    }
    buffer_size = size;
}

tag_list::tag_list(std::initializer_list<std::string_view> list) {
    for (std::string_view item : list) {
        for (ssize_t i = 0; i < item.size(); i++) {
            table.emplace(item.substr(0, i), false);
        }
    }

    size_t size = 0;
    for (const std::string_view item : list) {
        table.insert_or_assign(std::string(item), true);
        size = std::max(size, item.size());
    }
    buffer_size = size;
}

either<std::string, nullptr_t> tag_list::operator()(std::istream &is) const {
    std::string buffer;
    buffer.reserve(buffer_size);

    // rollback info
    std::string matched;
    matched.reserve(buffer_size);
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

        if (iter->second) {
            // update rollback
            position = is.tellg();
            matched = iter->first;
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

        if (iter->second) {
            // update rollback
            position = is.tellg();
            matched = iter->first;
        }
    } while (1);
}

tag_list_builder tag_list::builder() { return tag_list_builder(); }

std::ostream &operator<<(std::ostream &os, const tag_list &t) {

    // convert from table to ordered table to read by human, O(n log(n))
    std::map<std::string, bool> ordered_table;
    for (const auto &[k, v] : t.get_table()) {
        ordered_table[k] = v;
    }

    // output

    os << "tag_list: {";
    bool use_sep = false;
    for (const auto &[k, v] : ordered_table) {
        const char *boolean = v ? "true" : "false";
        if (use_sep) {
            os << ", ";
        }

        os << std::quoted(k) << ": " << boolean;
        use_sep = true;
    }
    os << " }";
    return os;
}

either<int, std::nullptr_t> digit_parser::operator()(std::istream &is) const {
    const unsigned int base = this->base;
    const auto to_int = [base](int d) -> std::optional<int> {
        if (base <= 10) {
            if ('0' <= d && d < '0' + base) {
                return d - '0';
            }
            return std::nullopt;
        } else {
            if ('0' <= d && d < '0' + 10) {
                return d - '0';
            }
            if ('a' <= d && d < 'a' + base - 10) {
                return d - 'a' + 0xa;
            }
            if ('A' <= d && d < 'A' + base - 10) {
                return d - 'A' + 0xA;
            }
            return std::nullopt;
        }
    };

    if (const auto x = to_int(is.peek()); x) {
        is.ignore();
        return right(*x);
    }
    return left(nullptr);
}

std::ostream &operator<<(std::ostream &os, const digit_parser &d) { return os << "digit(" << d.get_base() << ")"; }

static inline std::optional<char> escape(char c) {
    switch (c) {
    case 'a':
        return '\a';
    case 'b':
        return '\b';
    case 'f':
        return '\f';
    case 'n':
        return '\n';
    case 'r':
        return '\r';
    case 't':
        return '\t';
    case 'v':
        return '\v';
    case '?':
        return '\?';
    case '\'':
        return '\'';
    case '"':
        return '"';
    case '0':
        return 0;
    default:
        return std::nullopt;
    }
}

either<std::string, string_errors> string_parser::operator()(std::istream &is) const {
    const std::streampos pos = is.tellg();

    if (is.peek() != quote) {
        return left(string_errors::not_begin);
    }
    is.ignore();

    std::string result;
    while (is) {
        const int c = is.get();

        if (c == quote) {
            return right(result);
        } else if (c == '\\') {
            const int c2 = is.get();
            if (c2 == -1) {
                is.seekg(pos);
                return left(string_errors::not_end);
            }
            const auto e = escape(c2);
            if (!e) {
                is.seekg(pos);
                return left(string_errors::bad_escape);
            }
            result.push_back(*e);
        } else {
            result.push_back(c);
        }
    }

    is.seekg(pos);
    return left(string_errors::not_end);
}

either<std::string, raw_string_errors> raw_string_parser::operator()(std::istream &is) const {
    const std::streampos pos = is.tellg();

    // head
    if (quote(is).is_left()) {
        return left(raw_string_errors::not_begin);
    }

    std::string result;
    while (is) {
        if (quote(is).is_right()) {
            return right(result);
        }
        result.push_back(is.get());
    }

    is.seekg(pos);
    return left(raw_string_errors::not_end);
}

} // namespace tokenizes::primitive