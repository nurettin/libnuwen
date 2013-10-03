// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "test.hh"

#include "external_begin.hh"
    #include <cstdlib>
    #include <exception>
    #include <iostream>
    #include <new>
    #include <ostream>
    #include <stdexcept>
    #include <vector>
    #include <boost/lexical_cast.hpp>
#include "external_end.hh"

using namespace std;
using namespace boost;

bool logic() { throw logic_error("Borked logic."); }
bool runtime() { throw runtime_error("Borked runtime."); }
bool except() { throw bad_alloc(); }
bool unknown() { throw 0; }
bool bye() { exit(EXIT_FAILURE); }

int main(int argc, char * argv[]) {
    int option = 0;

    try {
        option = argc == 2 ? lexical_cast<int>(argv[1]) : 0;
    } catch (...) {
        option = 0;
    }

    switch (option) {
        case 1:
            NUWEN_TEST("dupe", true)
            NUWEN_TEST("dupe", true)
            break;
        case 2:
            NUWEN_TEST("untrue", false)
            break;
        case 3:
            NUWEN_TEST("logic", logic())
            break;
        case 4:
            NUWEN_TEST("runtime", runtime())
            break;
        case 5:
            NUWEN_TEST("except", except())
            break;
        case 6:
            NUWEN_TEST("unknown", unknown())
            break;
        case 7:
            NUWEN_TEST("bye", bye())
            break;
        case 8:
            {
            NUWEN_TEST("cute", true)
            NUWEN_TEST("fluffy", 5 * 5 == 25)

            vector<int> v;

            v.push_back(10);
            v.push_back(20);
            v.push_back(30);

            NUWEN_TEST("kittens", v.size() == 3)
            break;
            }
        default:
            cout << "USAGE: test_test <option>" << endl;
            cout << "    1: Duplicate test IDs." << endl;
            cout << "    2: Returns false." << endl;
            cout << "    3: Throws std::logic_error." << endl;
            cout << "    4: Throws std::runtime_error." << endl;
            cout << "    5: Throws std::exception." << endl;
            cout << "    6: Throws an unknown exception." << endl;
            cout << "    7: Exits unexpectedly." << endl;
            cout << "    8: Everything succeeds." << endl;
            break;
    }
}
