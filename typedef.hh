// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_TYPEDEF_HH
#define PHAM_TYPEDEF_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "external_begin.hh"
    #include <climits>
    #include <deque>
    #include <list>
    #include <string>
    #include <vector>
    #include <boost/cstdint.hpp>
#include "external_end.hh"

#if CHAR_BIT != 8
    #error This program requires 8-bit bytes.
#endif

namespace nuwen {
    typedef boost::uint8_t  uc_t;
    typedef boost::int8_t   sc_t;
    typedef boost::uint16_t us_t;
    typedef boost::int16_t  ss_t;
    typedef boost::uint32_t ul_t;
    typedef boost::int32_t  sl_t;
    typedef boost::uint64_t ull_t;
    typedef boost::int64_t  sll_t;

    #define PHAM_TYPEDEF_ALPHA(PHAM_LONG, PHAM_SHORT)                   \
        typedef PHAM_LONG                         PHAM_SHORT ## _t;     \
        typedef PHAM_LONG::iterator               PHAM_SHORT ## _i_t;   \
        typedef PHAM_LONG::const_iterator         PHAM_SHORT ## _ci_t;  \
        typedef PHAM_LONG::difference_type        PHAM_SHORT ## _d_t;   \
        typedef PHAM_LONG::size_type              PHAM_SHORT ## _s_t;   \
        typedef PHAM_LONG::reverse_iterator       PHAM_SHORT ## _ri_t;  \
        typedef PHAM_LONG::const_reverse_iterator PHAM_SHORT ## _cri_t;

    #define PHAM_TYPEDEF_BETA(PHAM_CONTAINER, PHAM_T, PHAM_PREFIX, PHAM_SIZE) \
        PHAM_TYPEDEF_ALPHA(PHAM_CONTAINER<PHAM_T>, PHAM_PREFIX ## PHAM_SIZE)

    #define PHAM_TYPEDEF_GAMMA(PHAM_T, PHAM_SIZE)            \
        PHAM_TYPEDEF_BETA(std::vector, PHAM_T, v, PHAM_SIZE) \
        PHAM_TYPEDEF_BETA(std::deque,  PHAM_T, d, PHAM_SIZE) \
        PHAM_TYPEDEF_BETA(std::list,   PHAM_T, l, PHAM_SIZE)

    PHAM_TYPEDEF_GAMMA(bool,        b)
    PHAM_TYPEDEF_GAMMA(char,        c)
    PHAM_TYPEDEF_GAMMA(std::string, s)
    PHAM_TYPEDEF_GAMMA(float,       f)
    PHAM_TYPEDEF_GAMMA(double,      d)

    #define PHAM_TYPEDEF_DELTA(PHAM_SIZE) \
        PHAM_TYPEDEF_GAMMA(PHAM_SIZE ## _t, PHAM_SIZE)

    PHAM_TYPEDEF_DELTA(uc)
    PHAM_TYPEDEF_DELTA(sc)
    PHAM_TYPEDEF_DELTA(us)
    PHAM_TYPEDEF_DELTA(ss)
    PHAM_TYPEDEF_DELTA(ul)
    PHAM_TYPEDEF_DELTA(sl)
    PHAM_TYPEDEF_DELTA(ull)
    PHAM_TYPEDEF_DELTA(sll)

    #undef PHAM_TYPEDEF_ALPHA
    #undef PHAM_TYPEDEF_BETA
    #undef PHAM_TYPEDEF_GAMMA
    #undef PHAM_TYPEDEF_DELTA
}

#endif // Idempotency
