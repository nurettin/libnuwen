// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "bwt.hh"
#include "clock.hh"
#include "file.hh"
#include "gluon.hh"
#include "memory.hh"
#include "string.hh"
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

bool core(const vuc_t& v, const vuc_t& correct) {
    const vuc_t b = bwt(v);

    return b == correct && unbwt(b) == v;
}

bool test_tiny() {
    return core(vuc_t(1, 88), vec(glu<uc_t>(0)(0)(0)(1)(0)(0)(0)(0)(0)(88)));
}

bool test_medium() {
    const uc_t I = 73;
    const uc_t M = 77;
    const uc_t P = 80;
    const uc_t S = 83;

    return core(vec(glu<uc_t>(M)(I)(S)(S)(I)(S)(S)(I)(P)(P)(I)),
        vec(glu<uc_t>(0)(0)(0)(2)(0)(0)(0)(4)(S)(S)(M)(P)(0)(P)(I)(S)(S)(I)(I)(I)));
}

bool test_big() {
    return core(vuc_t(3 * 1048576, 77), vec(glu<uc_t>(0)(0)(0)(1)(0)(0)(0)(0)(0)(vuc_t(3 * 1048576, 77))));
}

vuc_t instrumented_bwt(const vuc_t& v) {
    using namespace pham::ukk;

    if (v.size() < MIN_ALLOWED_SIZE) {
        throw logic_error("LOGIC ERROR: instrumented_bwt() - v is too small.");
    }

    if (v.size() > MAX_ALLOWED_SIZE) {
        throw runtime_error("RUNTIME ERROR: instrumented_bwt() - v is too big.");
    }

    vuc_t dest(v.size() + 9);

    const ull_t initial_usage = vm_bytes();

    ull_t memory_usage = 0;
    double ukkonen_time = 0;
    double dfs_time = 0;

    const watch total;

    {
        tree st(v);

        ukkonen_time = total.seconds();

        st.dfs(bwt_helper(v, dest));

        dfs_time = total.seconds() - ukkonen_time;

        memory_usage = vm_bytes() - initial_usage;
    }

    const double total_time = total.seconds();

    const double dtor_time = total_time - ukkonen_time - dfs_time;

    cout << "    File size (B): " << comma_from_ull(v.size())              << endl;
    cout << " Memory usage (B): " << comma_from_ull(memory_usage)          << endl;
    cout << "Memory factor (N): " << static_cast<double>(memory_usage) / static_cast<double>(v.size()) << endl;
    cout << "(Not including the input and output vectors.)"                   << endl;
    cout << "Ukkonen (s): " << ukkonen_time                                   << endl;
    cout << "    DFS (s): " <<     dfs_time                                   << endl;
    cout << "   Dtor (s): " <<    dtor_time                                   << endl;
    cout << "  Total (s): " <<   total_time                                   << endl;
    cout << "Ukkonen (KB/s): " << v.size() / ukkonen_time / 1024              << endl;
    cout << "    DFS (KB/s): " << v.size() /     dfs_time / 1024              << endl;
    cout << "   Dtor (KB/s): " << v.size() /    dtor_time / 1024              << endl;
    cout << "  Total (KB/s): " << v.size() /   total_time / 1024              << endl;
    cout << "Memory consumption (KB/s): " << static_cast<double>(memory_usage) / total_time / 1024 << endl;
    cout << endl;

    return dest;
}

bool test_instrumented(const string& s) {
    const vuc_t v = read_file(s);

    return unbwt(instrumented_bwt(v)) == v;
}

bool test_timing(const string& s) {
    const vuc_t v = read_file(s);


    watch w;

    const vuc_t b = bwt(v);

    const double bwt_time = w.seconds();


    w.reset();

    const vuc_t u = unbwt(b);

    const double unbwt_time = w.seconds();


    cout << "  BWT (s): " <<   bwt_time                      << endl;
    cout << "UnBWT (s): " << unbwt_time                      << endl;
    cout << "  BWT (KB/s): " << v.size() /   bwt_time / 1024 << endl;
    cout << "UnBWT (KB/s): " << v.size() / unbwt_time / 1024 << endl;


    return u == v;
}

int main(int argc, char * argv[]) {
    if (argc == 1) {
        NUWEN_TEST("bwt1", test_tiny())
        NUWEN_TEST("bwt2", test_medium())
        NUWEN_TEST("bwt3", test_big())
    } else if (argc == 2) {
        NUWEN_TEST("bwt4", test_instrumented(argv[1]))
        NUWEN_TEST("bwt5", test_timing(argv[1]))
    } else {
        cout << "USAGE: bwt_test            (for correctness)" << endl;
        cout << "USAGE: bwt_test <filename> (for profiling)"   << endl;
    }
}
