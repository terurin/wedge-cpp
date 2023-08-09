// #include "parsers.hpp"

#include "tokens.hpp"
#include <iostream>
#include <regex>
#include <sstream>
using namespace std;

int main(int argc, char **argv) {
    stringstream ss;
    //ss << "hello_world";
    ss << "-0b1111";
    char c;

    tokenizes::tokens::token_parser parser;

    if (auto e = parser(ss); e.is_right()) {
        cout << e.get_right();
    } else {
        cout << "failed" << endl;
    }
}