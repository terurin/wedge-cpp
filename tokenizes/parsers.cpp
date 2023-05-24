#include "parsers.hpp"
#include <ctype.h>
#include <iomanip>
#include <string.h>
namespace tokenizes {

std::ostream &operator<<(std::ostream &os, const escaped_char &ec) {
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
        if (!chars.test(i))
            continue;

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

std::shared_ptr<atom> atom::from(const chars_t &chars) { return std::make_shared<atom>(chars); }
std::shared_ptr<atom> atom::from(uint8_t c) {
    chars_t chars;
    chars.set(c);
    return std::make_shared<atom>(chars);
}

std::shared_ptr<atom> atom::from(std::string_view sv) {
    chars_t chars;
    for (unsigned char c : sv)
        chars.set(c);

    return std::make_shared<atom>(chars);
}

std::shared_ptr<atom> atom::from_range(uint8_t first, uint8_t last) {
    chars_t chars;
    for (size_t c = first; c <= last; c++)
        chars.set(c);

    return std::make_shared<atom>(chars);
}

bool atom::operator()(std::stringstream &ss, char &c, empty_t &e) const {
    c = ss.peek();
    if (c == -1 || !chars.test(c)) {
        return false;
    }

    ss.ignore();
    return true;
}

bool atom::operator()(std::stringstream &ss, std::string &s, empty_t &) const {
    const char c = ss.peek();
    if (c == -1 || !chars.test(c)) {
        return false;
    }

    ss.ignore();
    s.push_back(c);
    return true;
}

atom_ptr operator|(const atom_ptr &a, const atom_ptr &b) { return atom::from(a->get_chars() | b->get_chars()); }
atom_ptr operator|=(atom_ptr &a, const atom_ptr &b) { return a->get_chars() |= b->get_chars(), a; }
atom_ptr operator&(const atom_ptr &a, const atom_ptr &b) { return atom::from(a->get_chars() & b->get_chars()); }
atom_ptr &operator&=(atom_ptr &a, const atom_ptr &b) { return a->get_chars() &= b->get_chars(), a; }
atom_ptr operator^(const atom_ptr &a, const atom_ptr &b) { return atom::from(a->get_chars() ^ b->get_chars()); }
atom_ptr &operator^=(atom_ptr &a, const atom_ptr &b) { return a->get_chars() ^= b->get_chars(), a; }
atom_ptr operator~(const atom_ptr &a) { return atom::from(~a->get_chars()); }

} // namespace tokenizes