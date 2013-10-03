// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "bzip2.hh"
#include "clock.hh"
#include "file.hh"
#include "test.hh"
#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <iostream>
    #include <ostream>
    #include <string>
#include "external_end.hh"

using namespace std;
using namespace nuwen;
using namespace nuwen::chrono;
using namespace nuwen::file;

bool test_helper(const vuc_t& orig, const vuc_t& correct) {
    return bzip2(orig) == correct && unbzip2(correct) == orig;
}

bool test_empty() {
    return test_helper(vuc_t(), vuc_from_hex("425A683917724538509000000000"));
}

bool test_basic() {
    return test_helper(vuc_from_hex("48656C6C6F2C20776F726C6421"),
        vuc_from_hex("425A68393141592653598E9A770600000195806004004006"
            "0490802000220346210030B041E4B21F3F1772453850908E9A7706"));
}

bool test_extended(const string& filename) {
    const vuc_t orig = read_file(filename);


    watch w;

    const vuc_t& compressed = bzip2(orig);

    const double bzip2_time = w.seconds();


    w.reset();

    const vuc_t& decompressed = unbzip2(compressed);

    const double unbzip2_time = w.seconds();


    cout << "  bzip2 (s): " <<   bzip2_time << endl;
    cout << "unbzip2 (s): " << unbzip2_time << endl;
    cout << "  bzip2 (MB/s): " << orig.size() /   bzip2_time / 1048576 << endl;
    cout << "unbzip2 (MB/s): " << orig.size() / unbzip2_time / 1048576 << endl;

    return decompressed == orig;
}

int main(int argc, char * argv[]) {
    if (argc == 2) {
        NUWEN_TEST("bzip2-1", test_empty())
        NUWEN_TEST("bzip2-2", test_basic())
        NUWEN_TEST("bzip2-3", test_extended(argv[1]))
    } else {
        cout << "USAGE: bzip2_test <filename>" << endl;
    }
}
