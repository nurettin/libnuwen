// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "arith.hh"
#include "bwt.hh"
#include "clock.hh"
#include "file.hh"
#include "gluon.hh"
#include "mtf.hh"
#include "test.hh"
#include "typedef.hh"
#include "zle.hh"

#include "external_begin.hh"
    #include <iostream>
    #include <ostream>
    #include <stdexcept>
    #include <string>
    #include <boost/format.hpp>
#include "external_end.hh"

using namespace std;
using namespace boost;
using namespace nuwen;
using namespace nuwen::chrono;
using namespace nuwen::file;

bool test_vector(const vuc_t& v) {
    return unarith(arith(v)) == v;
}

bool test_empty() {
    return test_vector(vuc_t());
}

bool test_rainbow() {
    vuc_t v;

    for (int i = 0; i < 256; ++i) {
        v += cat(vuc_t(i + 10, i));
    }

    return test_vector(v);
}

bool test_huge() {
    return test_vector(vec(cat(vuc_t(1000000, 0x00))(vuc_t(1000000, 0x01))(vuc_t(1000000, 0xFF))));
}

void test_helper(const vuc_t& orig) {
    watch w;
    const vuc_t& a = arith(orig);
    const double arith_time = w.seconds();

    w.reset();
    const vuc_t& unarithed = unarith(a);
    const double unarith_time = w.seconds();

    if (unarithed != orig) {
        throw runtime_error("RUNTIME ERROR: test_helper() - Mangled data.");
    }

    cout << " Original Size: " << orig.size()                          << endl;
    cout << "  Arithed Size: " << a.size()                             << endl;
    cout << " Bits Per Byte: " << 8.0 * a.size() / orig.size()         << endl;
    cout << "  Arith (MB/s): " << orig.size() /   arith_time / 1048576 << endl;
    cout << "Unarith (MB/s): " << orig.size() / unarith_time / 1048576 << endl;
}

bool test_file(const string& filename) {
    const vuc_t orig_file = read_file(filename);

    cout << "[Plain File]" << endl;
    test_helper(orig_file);

    cout << endl;

    vuc_t v = bwt(orig_file);
    mtf2(v);
    v = zle(v);

    cout << "[BWT/MTF-2/ZLEd File]" << endl;
    test_helper(v);

    return true;
}

int main(int argc, char * argv[]) {
    if (argc == 2) {
        cout << format("Triple: (%1%, %2%, %3%)")
            % pham::arith::CODE_VALUE_BITS
            % pham::arith::MAX_FREQUENCY
            % pham::arith::INCREMENT_VALUE << endl << endl;

        NUWEN_TEST("arith1", test_empty())
        NUWEN_TEST("arith2", test_rainbow())
        NUWEN_TEST("arith3", test_huge())
        NUWEN_TEST("arith4", test_file(argv[1]))
    } else {
        cout << "USAGE: arith_test <filename>" << endl;
    }
}
