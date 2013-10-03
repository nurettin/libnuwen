// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "clock.hh"
#include "file.hh"
#include "test.hh"
#include "typedef.hh"
#include "vector.hh"
#include "zlib.hh"

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
    return zlib(orig) == correct && unzlib(correct) == orig;
}

bool test_empty() {
    return test_helper(vuc_t(), vuc_from_hex("78DA030000000001"));
}

bool test_basic() {
    return test_helper(vuc_from_hex("48656C6C6F2C20776F726C6421"), vuc_from_hex("78DAF348CDC9C9D75128CF2FCA49510400205E048A"));
}

bool test_extended(const string& filename) {
    const vuc_t orig = read_file(filename);


    watch w;

    const vuc_t& compressed = zlib(orig);

    const double zlib_time = w.seconds();


    w.reset();

    const vuc_t& decompressed = unzlib(compressed);

    const double unzlib_time = w.seconds();


    cout << "  zlib (s): " <<   zlib_time << endl;
    cout << "unzlib (s): " << unzlib_time << endl;
    cout << "  zlib (MB/s): " << orig.size() /   zlib_time / 1048576 << endl;
    cout << "unzlib (MB/s): " << orig.size() / unzlib_time / 1048576 << endl;

    return decompressed == orig;
}

int main(int argc, char * argv[]) {
    if (argc == 2) {
        NUWEN_TEST("zlib1", test_empty())
        NUWEN_TEST("zlib2", test_basic())
        NUWEN_TEST("zlib3", test_extended(argv[1]))
    } else {
        cout << "USAGE: zlib_test <filename>" << endl;
    }
}
