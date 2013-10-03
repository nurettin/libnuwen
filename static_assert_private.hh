// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_STATIC_ASSERT_PRIVATE_HH
#define PHAM_STATIC_ASSERT_PRIVATE_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "external_begin.hh"
    #include <boost/static_assert.hpp>
#include "external_end.hh"

#ifdef NUWEN_PLATFORM_GCC
    // Modified from boost/static_assert.hpp RC_1_34_0 20061231, which is Copyright John Maddock.
    // The old-style casts to bool have been removed, as they trigger -Wold-style-cast.
    // static_cast<bool> seems to sometimes cause compilation problems, so
    // PHAM_STATIC_ASSERT simply requires that its argument actually be a bool.

    #ifdef BOOST_SA_GCC_WORKAROUND
        #define PHAM_STATIC_ASSERT(B) \
            typedef char BOOST_JOIN(boost_static_assert_typedef_, __LINE__) \
            [::boost::STATIC_ASSERTION_FAILURE<(B)>::value]
    #else
        #define PHAM_STATIC_ASSERT(B) \
            typedef ::boost::static_assert_test<sizeof(::boost::STATIC_ASSERTION_FAILURE<(B)>)> \
            BOOST_JOIN(boost_static_assert_typedef_, __LINE__)
    #endif
#else
    #define PHAM_STATIC_ASSERT BOOST_STATIC_ASSERT
#endif

#endif // Idempotency
