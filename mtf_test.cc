// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "bwt.hh"
#include "clock.hh"
#include "file.hh"
#include "gluon.hh"
#include "mtf.hh"
#include "test.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <iostream>
    #include <ostream>
    #include <string>
#include "external_end.hh"

using namespace std;
using namespace nuwen;
using namespace nuwen::chrono;
using namespace nuwen::file;

bool test_basic() {
    const vuc_t orig = vec(glu<uc_t>(13)(10)(14)(0)(10)(5)(6)(1)(9)(10)(1)(3)(9)(1)(1)(6)(4)(2)(7)(0)(8)(1)(1)(8)(8)(8));
    const vuc_t correct = vec(glu<uc_t>(13)(11)(14)(0)(2)(8)(9)(6)(12)(5)(3)(9)(4)(3)(1)(5)(10)(10)(11)(5)(12)(0)(0)(1)(1)(0));

    vuc_t v = orig;

    mtf2(v);

    const vuc_t mtf2ed = v;

    unmtf2(v);

    return mtf2ed == correct && v == orig;
}

bool test_extended(const string& filename) {
    const vuc_t orig = bwt(read_file(filename));

    vuc_t v = orig;


    watch w;

    mtf2(v);

    const double mtf2_time = w.seconds();


    w.reset();

    unmtf2(v);

    const double unmtf2_time = w.seconds();


    cout << "  MTF-2 (s): " <<   mtf2_time << endl;
    cout << "UnMTF-2 (s): " << unmtf2_time << endl;
    cout << "  MTF-2 (MB/s): " << orig.size() /   mtf2_time / 1048576 << endl;
    cout << "UnMTF-2 (MB/s): " << orig.size() / unmtf2_time / 1048576 << endl;

    return v == orig;
}

int main(int argc, char * argv[]) {
    if (argc == 2) {
        NUWEN_TEST("mtf1", test_basic())
        NUWEN_TEST("mtf2", test_extended(argv[1]))
    } else {
        cout << "USAGE: mtf_test <filename>" << endl;
    }
}
