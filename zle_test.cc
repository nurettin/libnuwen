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
#include "zle.hh"

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
    const vuc_t orig = vec(glu<uc_t>(0)(0xFE)(0)(0)(0xFF)(0)(0)(0)(0x01)(0xCC)(0xFD)(vuc_t(137, 0)));
    const vuc_t correct = vec(glu<uc_t>(0)(0xFF)(0x00)(1)(0xFF)(0x01)(0)(0)(0x02)(0xCD)(0xFE)(0)(1)(0)(1)(0)(0)(0));

    const vuc_t& z = zle(orig);

    return z == correct && unzle(z) == orig;
}

bool test_extended(const string& filename) {
    vuc_t orig = bwt(read_file(filename));
    mtf2(orig);


    watch w;

    const vuc_t& z = zle(orig);

    const double zle_time = w.seconds();


    w.reset();

    const vuc_t& u = unzle(z);

    const double unzle_time = w.seconds();


    cout << "  ZLE (s): " <<   zle_time << endl;
    cout << "UnZLE (s): " << unzle_time << endl;
    cout << "  ZLE (MB/s): " << orig.size() /   zle_time / 1048576 << endl;
    cout << "UnZLE (MB/s): " << orig.size() / unzle_time / 1048576 << endl;

    return u == orig;
}

int main(int argc, char * argv[]) {
    if (argc == 2) {
        NUWEN_TEST("zle1", test_basic())
        NUWEN_TEST("zle2", test_extended(argv[1]))
    } else {
        cout << "USAGE: zle_test <filename>" << endl;
    }
}
