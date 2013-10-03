// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "clock.hh"
#include "gluon.hh"
#include "sha256.hh"
#include "test.hh"
#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <fstream>
    #include <iostream>
    #include <ostream>
    #include <string>
    #include <utility>
    #include <vector>
    #include <boost/format.hpp>
    #include <boost/regex.hpp>
#include "external_end.hh"

using namespace std;
using namespace boost;
using namespace nuwen;
using namespace nuwen::chrono;

bool test_hex(const string& data, const string& hash) {
    return sha256(vuc_from_hex(data)) == vuc_from_hex(hash);
}

bool test_speed() {
    const int megs = 100;

    const vuc_t v(megs * 1048576, 97);

    const watch w;

    const vuc_t& hash = sha256(v);

    const double sha256_time = w.seconds();

    cout << "Data Size (MB): " << megs               << endl;
    cout << "SHA-256 (s):    " << sha256_time        << endl;
    cout << "SHA-256 (MB/s): " << megs / sha256_time << endl;

    // This reference hash was calculated by Brian Gladman's implementation (http://fp.gladman.plus.com),
    // which is completely independent from mine.
    return hash == vuc_from_hex("CEE41E98D0A6AD65CC0EC77A2BA50BF26D64DC9007F7F1C7D7DF68B8B71291A6");
}

typedef vector<pair<string, string> > vpss_t;
typedef vpss_t::const_iterator vpss_ci_t;

string strip_name(const string& s) {
    return regex_replace(s, regex(".* = "), "");
}

bool read_vectors(ifstream& file, vpss_t& output, const vs_s_t n) {
    vs_t len;
    vs_t msg;
    vs_t md;

    for (string line; getline(file, line); ) {
        if (regex_match(line, regex("Len = .*"))) {
            len.push_back(strip_name(line));
        } else if (regex_match(line, regex("Msg = .*"))) {
            msg.push_back(strip_name(line));
        } else if (regex_match(line, regex("MD = .*"))) {
            md.push_back(strip_name(line));
        }
    }

    if (len.size() != n || msg.size() != n || md.size() != n) {
        return false;
    }

    for (vs_s_t i = 0; i < len.size(); ++i) {
        output.push_back(make_pair(len[i] == "0" ? "" : msg[i], md[i]));
    }

    return true;
}

void read_monte(ifstream& file, vuc_t& seed, vs_t& output) {
    for (string line; getline(file, line); ) {
        if (regex_match(line, regex("Seed = .*"))) {
            seed = vuc_from_hex(strip_name(line));
        } else if (regex_match(line, regex("MD = .*"))) {
            output.push_back(strip_name(line));
        }
    }
}

int main() {
    // Test the FIPS 180-2 examples.
    NUWEN_TEST("sha256-1", test_hex("616263", "BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD"))

    NUWEN_TEST("sha256-2", test_hex(
        "6162636462636465636465666465666765666768666768696768696A"
        "68696A6B696A6B6C6A6B6C6D6B6C6D6E6C6D6E6F6D6E6F706E6F7071",
        "248D6A61D20638B8E5C026930C3E6039A33CE45964FF2167F6ECEDD419DB06C1"))

    NUWEN_TEST("sha256-3", sha256(vuc_t(1000000, 97)) == vuc_from_hex(
        "CDC76E5C9914FB9281A1C7E284D73E67F1809A48A497200E046D39CCC7112CD0"))

    // Test a huge example and also gather timing information.
    NUWEN_TEST("sha256-4", test_speed())

    ifstream shortmsg("SHA256ShortMsg.txt");
    ifstream longmsg("SHA256LongMsg.txt");
    ifstream monte("SHA256Monte.txt");

    if (shortmsg && longmsg && monte) {
        vpss_t nist_vectors;

        NUWEN_TEST("sha256-5", read_vectors(shortmsg, nist_vectors, 65))
        NUWEN_TEST("sha256-6", read_vectors(longmsg, nist_vectors, 64))

        for (vpss_ci_t i = nist_vectors.begin(); i != nist_vectors.end(); ++i) {
            const string id = str(format("sha256-7-%1%") % (i - nist_vectors.begin()));

            NUWEN_TEST(id, test_hex(i->first, i->second))
        }


        vuc_t seed;
        vs_t nist_monte;

        read_monte(monte, seed, nist_monte);

        NUWEN_TEST("sha256-8", nist_monte.size() == 100)

        vector<vuc_t> md(1003);

        for (int j = 0; j < 100; ++j) {
            md[0] = md[1] = md[2] = seed;

            for (vector<vuc_t>::size_type i = 3; i < 1003; ++i) {
                md[i] = sha256(vec(cat(md[i - 3])(md[i - 2])(md[i - 1])));
            }

            seed = md[1002];

            const string id = str(format("sha256-9-%1%") % j);

            NUWEN_TEST(id, seed == vuc_from_hex(nist_monte[j]))
        }

        cout << "Extended tests of 129 NIST test vectors and 100 Monte Carlo samples succeeded." << endl;
    } else {
        cout << "Basic tests succeeded. To run extended tests, get the NIST test vectors at:" << endl;
        cout << "http://csrc.nist.gov/cryptval/shs/shabytetestvectors.zip" << endl;
    }
}
