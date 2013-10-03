// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "test.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <climits>
#include "external_end.hh"

using namespace nuwen;

int main() {
    NUWEN_TEST("typedef1", sizeof(uc_t)  * CHAR_BIT == 8 )
    NUWEN_TEST("typedef2", sizeof(us_t)  * CHAR_BIT == 16)
    NUWEN_TEST("typedef3", sizeof(ul_t)  * CHAR_BIT == 32)
    NUWEN_TEST("typedef4", sizeof(ull_t) * CHAR_BIT == 64)

    NUWEN_TEST("typedef5", sizeof(sc_t)  * CHAR_BIT == 8 )
    NUWEN_TEST("typedef6", sizeof(ss_t)  * CHAR_BIT == 16)
    NUWEN_TEST("typedef7", sizeof(sl_t)  * CHAR_BIT == 32)
    NUWEN_TEST("typedef8", sizeof(sll_t) * CHAR_BIT == 64)
}
