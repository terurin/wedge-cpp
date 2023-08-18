// #include "parsers.hpp"

#include "tokens.hpp"
#include <iostream>
#include <regex>
#include <sstream>
using namespace std;

int main(int argc, char **argv) {
    stringstream ss;
    // ss << "hello_world";
    ss << "x";
    char c;

    tokenizes::tokens::token_parser parser;

    using tokenizes::eithers::either_mode;
    switch (auto e = parser(ss); e.get_mode()) {
    case either_mode::right:
        cout << e.get_right();
        break;
    case either_mode::left:
        cout << e.get_left();
        break;
    default:
        break;
    }
}