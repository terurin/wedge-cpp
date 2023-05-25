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
    // empty_t empty;
    string s;

    if ((*repeat_n_m(atom::create('a'), 1, 2))(ss, s)) {
        cout << "pass:" << s;
    } else {
        cout << "fail:" << s;
    }
}