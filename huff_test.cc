// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#define PHAM_AUTOMATON_TIMING

#include "bwt.hh"
#include "clock.hh"
#include "file.hh"
#include "huff.hh"
#include "mtf.hh"
#include "test.hh"
#include "typedef.hh"
#include "zle.hh"

#include "external_begin.hh"
    #include <iostream>
    #include <ostream>
    #include <stdexcept>
    #include <string>
#include "external_end.hh"

using namespace std;
using namespace nuwen;
using namespace nuwen::chrono;
using namespace nuwen::file;

typedef vuc_t (*fxnptr_t)(const vuc_t&);

bool test_empty(const fxnptr_t h_fxn, const fxnptr_t p_fxn) {
    const vuc_t compressed = h_fxn(vuc_t());

    return compressed.size() == 256 && p_fxn(compressed).empty();
}

bool test_big(const fxnptr_t h_fxn, const fxnptr_t p_fxn) {
    const vuc_t orig(1234567, 137);

    const vuc_t compressed = h_fxn(orig);

    return compressed.size() == 154577 && p_fxn(compressed) == orig;
}

void test_helper(const vuc_t& orig) {
    watch w;
    const vuc_t& hb = pham::huff::huff_bits(orig);
    const double hb_time = w.seconds();

    w.reset();
    const vuc_t& pb = pham::huff::puff_bits(hb);
    const double pb_time = w.seconds();

    double huff_ctor_time = 0;

    w.reset();
    const vuc_t& ha = pham::huff::huff_auto(orig, &huff_ctor_time);
    const double ha_time = w.seconds();

    double puff_ctor_time = 0;

    w.reset();
    const vuc_t& pa = pham::huff::puff_auto(ha, &puff_ctor_time);
    const double pa_time = w.seconds();

    if (pb != orig) {
        throw runtime_error("RUNTIME ERROR: test_helper() - pb is mangled.");
    }

    if (pa != orig) {
        throw runtime_error("RUNTIME ERROR: test_helper() - pa is mangled.");
    }

    if (hb != ha) {
        throw runtime_error("RUNTIME ERROR: test_helper() - hb and ha should be identical.");
    }

    if (huff(orig) != hb) {
        throw runtime_error("RUNTIME ERROR: test_helper() - nuwen::huff() failed.");
    }

    if (puff(hb) != orig) {
        throw runtime_error("RUNTIME ERROR: test_helper() - nuwen::puff() failed.");
    }

    cout << "Original Size:    " << orig.size()                     << endl;
    cout << "Huffed Size:      " << hb.size()                       << endl;
    cout << "Bits Per Byte:    " << 8.0 * hb.size() / orig.size()   << endl;
    cout << "Huff Bits (MB/s): " << orig.size() / hb_time / 1048576 << endl;
    cout << "Puff Bits (MB/s): " << orig.size() / pb_time / 1048576 << endl;
    cout << "Huff Auto (MB/s): " << orig.size() / ha_time / 1048576 << endl;
    cout << "Puff Auto (MB/s): " << orig.size() / pa_time / 1048576 << endl;
    cout << "Huff Ctor (ms):   " << huff_ctor_time * 1000           << endl;
    cout << "Puff Ctor (ms):   " << puff_ctor_time * 1000           << endl;
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

vuc_t huff_auto_single(const vuc_t& v) {
    return pham::huff::huff_auto(v);
}

vuc_t puff_auto_single(const vuc_t& v) {
    return pham::huff::puff_auto(v);
}

int main(int argc, char * argv[]) {
    if (argc == 2) {
        NUWEN_TEST("huff1", test_empty(pham::huff::huff_bits, pham::huff::puff_bits))
        NUWEN_TEST("huff2", test_empty(huff_auto_single, puff_auto_single))
        NUWEN_TEST("huff3", test_empty(huff, puff))

        NUWEN_TEST("huff4", test_big(pham::huff::huff_bits, pham::huff::puff_bits))
        NUWEN_TEST("huff5", test_big(huff_auto_single, puff_auto_single))
        NUWEN_TEST("huff6", test_big(huff, puff))

        NUWEN_TEST("huff7", test_file(argv[1]))
    } else {
        cout << "USAGE: huff_test <filename>" << endl;
    }
}
