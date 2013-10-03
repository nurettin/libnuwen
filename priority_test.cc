// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "clock.hh"
#include "priority.hh"
#include "test.hh"

#include "external_begin.hh"
    #include <iostream>
    #include <ostream>
#include "external_end.hh"

using namespace std;
using namespace nuwen;

int main() {
    cout << "Priority: Normal." << endl;
    cout << "Snoozing for 10 seconds..." << endl;
    snooze(10);

    NUWEN_TEST("priority1", (set_priority_idle(), true))

    cout << "Priority: Idle." << endl;
    cout << "Snoozing for 10 seconds..." << endl;
    snooze(10);
}
