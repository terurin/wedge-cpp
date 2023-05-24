#include "parsers.hpp"
#include <iostream>
#include <regex>
#include <sstream>
using namespace std;
using namespace tokenizes;
int main(int argc, char **argv) {
    stringstream ss;
    ss << "aa";
    ss.seekg(0);
    char c;
    empty_t empty;
    string s;

    if (repeat_string<empty_t>(atom::from('a'), 1)(ss, s, empty)) {
        cout << "pass:" << s;
    } else {
        cout << "fail:" << s;
    }
}