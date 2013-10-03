// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "random.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <iostream>
    #include <ostream>
    #include <boost/format.hpp>
#include "external_end.hh"

using namespace std;
using namespace boost;
using namespace nuwen;
using namespace nuwen::random;

int main() {
    twister t;

    cout << "Here are 10 random 8, 16, 32, and 64-bit values:" << endl;

    for (int i = 0; i < 10; ++i) {
        const uc_t  uc  = t.random_uc();
        const us_t  us  = t.random_us();
        const ul_t  ul  = t.random_ul();
        const ull_t ull = t.random_ull();

        cout << format("0x%02X 0x%04X 0x%08X 0x%016X") % static_cast<us_t>(uc) % us % ul % ull << endl;
    }

    cout << endl << "And here are 10 random floats and doubles in [0, 1]:" << endl;

    for (int i = 0; i < 10; ++i) {
        const float  f = t.random_float_0_1();
        const double d = t.random_double_0_1();

        cout << format("%1% %2%") % f % d << endl;
    }
}
