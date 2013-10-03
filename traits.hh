// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_TRAITS_HH
#define PHAM_TRAITS_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "typedef.hh"

#include "external_begin.hh"
    #include <cstddef>
    #include <deque>
    #include <list>
    #include <string>
    #include <vector>
    #include <boost/type_traits.hpp>
#include "external_end.hh"

namespace nuwen {
    template <typename X> struct is_sequence                  { static const bool value = false; };
    template <typename T> struct is_sequence<std::vector<T> > { static const bool value = true;  };
    template <typename T> struct is_sequence<std::deque<T>  > { static const bool value = true;  };
    template <typename T> struct is_sequence<std::list<T>   > { static const bool value = true;  };

    template <typename X, typename T> struct is_sequence_of        { static const bool value = false; };
    template <typename T> struct is_sequence_of<std::vector<T>, T> { static const bool value = true;  };
    template <typename T> struct is_sequence_of<std::deque<T>,  T> { static const bool value = true;  };
    template <typename T> struct is_sequence_of<std::list<T>,   T> { static const bool value = true;  };

    template <typename X> struct is_string {
        static const bool value = boost::is_same<X, std::string>::value;
    };

    template <typename X> struct is_stringlike_sequence {
        static const bool value =
               is_sequence_of<X, char>::value
            || is_sequence_of<X, uc_t>::value
            || is_sequence_of<X, sc_t>::value;
    };

    template <typename X, typename T>    struct is_bounded_array_of                { static const bool value = false; };
    template <typename T, std::size_t N> struct is_bounded_array_of<      T[N], T> { static const bool value = true;  };
    template <typename T, std::size_t N> struct is_bounded_array_of<const T[N], T> { static const bool value = true;  };
}

#endif // Idempotency
