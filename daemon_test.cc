// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "clock.hh"
#include "daemon.hh"

using namespace nuwen;

int main() {
    daemonize();
    snooze(10);
}
