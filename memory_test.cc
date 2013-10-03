// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "memory.hh"
#include "string.hh"
#include "test.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <iostream>
    #include <ostream>
#include "external_end.hh"

using namespace std;
using namespace nuwen;

bool test_vm_bytes() {
    const ull_t N = 10000000;

    const ull_t initial = vm_bytes();

    const vuc_t v(N);

    const ull_t final = vm_bytes();

    cout << " Constructed a vuc_t of size: " << comma_from_ull(N      ) << " B." << endl;
    cout << "Initial virtual memory usage: " << comma_from_ull(initial) << " B." << endl;
    cout << "  Final virtual memory usage: " << comma_from_ull(final  ) << " B." << endl;

    return true;
}

int main() {
    NUWEN_TEST("memory1", test_vm_bytes())
}
