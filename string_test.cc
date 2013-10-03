// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "string.hh"
#include "test.hh"

#include "external_begin.hh"
    #include <string>
#include "external_end.hh"

using namespace std;
using namespace nuwen;

int main() {
    NUWEN_TEST("string1", strip_outside_whitespace("") == "")
    NUWEN_TEST("string2", strip_outside_whitespace("    ") == "")

    NUWEN_TEST("string3", strip_outside_whitespace("x") == "x")
    NUWEN_TEST("string4", strip_outside_whitespace("    x") == "x")
    NUWEN_TEST("string5", strip_outside_whitespace("x    ") == "x")
    NUWEN_TEST("string6", strip_outside_whitespace("  x  ") == "x")

    NUWEN_TEST("string7", strip_outside_whitespace("mlar") == "mlar")
    NUWEN_TEST("string8", strip_outside_whitespace("    mlar") == "mlar")
    NUWEN_TEST("string9", strip_outside_whitespace("mlar    ") == "mlar")
    NUWEN_TEST("string10", strip_outside_whitespace("  mlar  ") == "mlar")

    NUWEN_TEST("string11", strip_outside_whitespace("Cute Fluffy Kittens") == "Cute Fluffy Kittens")
    NUWEN_TEST("string12", strip_outside_whitespace("    Cute Fluffy Kittens") == "Cute Fluffy Kittens")
    NUWEN_TEST("string13", strip_outside_whitespace("Cute Fluffy Kittens    ") == "Cute Fluffy Kittens")
    NUWEN_TEST("string14", strip_outside_whitespace("  Cute Fluffy Kittens  ") == "Cute Fluffy Kittens")

    string s = "cats";
    find_and_replace_mutate(s, "cat", "dog");
    NUWEN_TEST("string15", s == "dogs")

    NUWEN_TEST("string16", find_and_replace_copy("Mary had a little lamb.", "lamb", "cow") == "Mary had a little cow.")
    NUWEN_TEST("string17", find_and_replace_copy("Mooo", "o", "abc") == "Mabcabcabc")
    NUWEN_TEST("string18", find_and_replace_copy("thinking", "thinking", "think") == "think")
    NUWEN_TEST("string19", find_and_replace_copy("thinkinging", "thinking", "think") == "thinking")
    NUWEN_TEST("string20", find_and_replace_copy("thinkingthinking", "thinking", "think") == "thinkthink")
    NUWEN_TEST("string21", find_and_replace_copy("Mooo", "o", "oo") == "Moooooo")

    NUWEN_TEST("string22", upper("bleah") == "BLEAH")
    NUWEN_TEST("string23", upper("OinkFoozle") == "OINKFOOZLE")
    NUWEN_TEST("string24", upper("SLAW") == "SLAW")

    NUWEN_TEST("string25", lower("meow") == "meow")
    NUWEN_TEST("string26", lower("OinkFoozle") == "oinkfoozle")
    NUWEN_TEST("string27", lower("HSSK") == "hssk")

    NUWEN_TEST("string28", pham::low_three_digits_from_ull(0)       ==   "0")
    NUWEN_TEST("string29", pham::low_three_digits_from_ull(47)      ==  "47")
    NUWEN_TEST("string30", pham::low_three_digits_from_ull(137)     == "137")
    NUWEN_TEST("string31", pham::low_three_digits_from_ull(999)     == "999")
    NUWEN_TEST("string32", pham::low_three_digits_from_ull(1000)    == "000")
    NUWEN_TEST("string33", pham::low_three_digits_from_ull(1047)    == "047")
    NUWEN_TEST("string34", pham::low_three_digits_from_ull(1234567) == "567")

    NUWEN_TEST("string35", comma_from_ull(0) == "0")
    NUWEN_TEST("string36", comma_from_ull(47) == "47")
    NUWEN_TEST("string37", comma_from_ull(999) == "999")
    NUWEN_TEST("string38", comma_from_ull(1000) == "1,000")
    NUWEN_TEST("string39", comma_from_ull(1047) == "1,047")
    NUWEN_TEST("string40", comma_from_ull(12345) == "12,345")
    NUWEN_TEST("string41", comma_from_ull(1234567) == "1,234,567")
    NUWEN_TEST("string42", comma_from_ull(0xBEAD1729CAFE2161ULL) == "13,739,663,506,444,001,633")

    NUWEN_TEST("string43", comma_from_sll(0) == "0")
    NUWEN_TEST("string44", comma_from_sll(12345) == "12,345")
    NUWEN_TEST("string45", comma_from_sll(-1) == "-1")
    NUWEN_TEST("string46", comma_from_sll(-12345) == "-12,345")
    NUWEN_TEST("string47", comma_from_sll(-9223372036854775807LL - 1) == "-9,223,372,036,854,775,808")
}
