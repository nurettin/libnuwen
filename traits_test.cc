// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "test.hh"
#include "traits.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <string>
#include "external_end.hh"

using namespace std;
using namespace nuwen;

int main() {
    NUWEN_TEST("traits1", !is_sequence<int   >::value)
    NUWEN_TEST("traits2",  is_sequence<vuc_t >::value)
    NUWEN_TEST("traits3",  is_sequence<dull_t>::value)
    NUWEN_TEST("traits4",  is_sequence<lss_t >::value)
    NUWEN_TEST("traits5", !is_sequence<string>::value)

    NUWEN_TEST("traits6",  (!is_sequence_of<int,    int  >::value))
    NUWEN_TEST("traits7",  ( is_sequence_of<vuc_t,  uc_t >::value))
    NUWEN_TEST("traits8",  ( is_sequence_of<dull_t, ull_t>::value))
    NUWEN_TEST("traits9",  ( is_sequence_of<lss_t,  ss_t >::value))
    NUWEN_TEST("traits10", (!is_sequence_of<string, char >::value))
    NUWEN_TEST("traits11", (!is_sequence_of<vul_t,  sl_t >::value))

    NUWEN_TEST("traits12", !is_string<int   >::value)
    NUWEN_TEST("traits13",  is_string<string>::value)

    NUWEN_TEST("traits14", !is_stringlike_sequence<int   >::value)
    NUWEN_TEST("traits15",  is_stringlike_sequence<vc_t  >::value)
    NUWEN_TEST("traits16",  is_stringlike_sequence<vuc_t >::value)
    NUWEN_TEST("traits17",  is_stringlike_sequence<vsc_t >::value)
    NUWEN_TEST("traits18", !is_stringlike_sequence<vus_t >::value)
    NUWEN_TEST("traits19",  is_stringlike_sequence<dc_t  >::value)
    NUWEN_TEST("traits20",  is_stringlike_sequence<duc_t >::value)
    NUWEN_TEST("traits21",  is_stringlike_sequence<dsc_t >::value)
    NUWEN_TEST("traits22", !is_stringlike_sequence<dus_t >::value)
    NUWEN_TEST("traits23",  is_stringlike_sequence<lc_t  >::value)
    NUWEN_TEST("traits24",  is_stringlike_sequence<luc_t >::value)
    NUWEN_TEST("traits25",  is_stringlike_sequence<lsc_t >::value)
    NUWEN_TEST("traits26", !is_stringlike_sequence<lus_t >::value)
    NUWEN_TEST("traits27", !is_stringlike_sequence<string>::value)

    NUWEN_TEST("traits28", (!is_bounded_array_of<   uc_t, uc_t>::value))
    NUWEN_TEST("traits29", (!is_bounded_array_of< double, uc_t>::value))
    NUWEN_TEST("traits30", (!is_bounded_array_of<  vuc_t, uc_t>::value))
    NUWEN_TEST("traits31", (!is_bounded_array_of< uc_t *, uc_t>::value))
    NUWEN_TEST("traits32", (!is_bounded_array_of<sc_t[ ], uc_t>::value))
    NUWEN_TEST("traits33", (!is_bounded_array_of<sc_t[5], uc_t>::value))
    NUWEN_TEST("traits34", (!is_bounded_array_of<uc_t[ ], uc_t>::value))
    NUWEN_TEST("traits35", ( is_bounded_array_of<uc_t[5], uc_t>::value))
    NUWEN_TEST("traits36", (!is_bounded_array_of<const sc_t[ ], uc_t>::value))
    NUWEN_TEST("traits37", (!is_bounded_array_of<const sc_t[5], uc_t>::value))
    NUWEN_TEST("traits38", (!is_bounded_array_of<const uc_t[ ], uc_t>::value))
    NUWEN_TEST("traits39", ( is_bounded_array_of<const uc_t[5], uc_t>::value))
}
