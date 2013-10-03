// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "fibonacci.hh"
#include "gluon.hh"
#include "test.hh"
#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <boost/iterator/counting_iterator.hpp>
#include "external_end.hh"

using namespace boost;
using namespace nuwen;

int main() {
    {
        const vuc_t v(make_counting_iterator(1), make_counting_iterator(256));
        const vuc_t f = fibonacci_encode(v);

        NUWEN_TEST("fibonacci1", fibonacci_decode<uc_t>(f) == v)
        NUWEN_TEST("fibonacci2", fibonacci_encode(sequence_cast<duc_t>(v)) == f)
        NUWEN_TEST("fibonacci3", fibonacci_encode(sequence_cast<luc_t>(v)) == f)
    }

    {
        const vus_t v(make_counting_iterator(1), make_counting_iterator(65536));

        NUWEN_TEST("fibonacci4", fibonacci_decode<us_t>(fibonacci_encode(v)) == v)
    }

    {
        const vul_t v = vec(glu<ul_t>(0x01234567UL)(0xFFFFFFFFUL)(0x17762161UL));

        NUWEN_TEST("fibonacci5", fibonacci_decode<ul_t>(fibonacci_encode(v)) == v)
    }

    {
        const vull_t v = vec(glu<ull_t>(0x0123456789ABCDEFULL)(0xFFFFFFFFFFFFFFFFULL)(0x2161CAFE1776BEEFULL));

        NUWEN_TEST("fibonacci6", fibonacci_decode<ull_t>(fibonacci_encode(v)) == v)
    }

    {
        const vus_t v = vec(glu<us_t>(137)(2161)(1776)(47));

        const vuc_t f = vuc_from_hex("8571253812182C");

        NUWEN_TEST("fibonacci7", fibonacci_encode(v) == f)
        NUWEN_TEST("fibonacci8", fibonacci_decode<us_t>(f) == v)
    }

    NUWEN_TEST("fibonacci9", fibonacci_decode<uc_t>(vuc_from_hex("FD80")) == vec(glu<uc_t>(1)(1)(1)(2)))
}
