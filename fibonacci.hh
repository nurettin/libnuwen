// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_FIBONACCI_HH
#define PHAM_FIBONACCI_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "static_assert_private.hh"
#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <stdexcept>
    #include <vector>
    #include <boost/type_traits.hpp>
#include "external_end.hh"

namespace nuwen {
    template <typename C> vuc_t fibonacci_encode(const C& c);
    template <typename T> std::vector<T> fibonacci_decode(const vuc_t& v);
}

namespace pham {
    template <typename T> const std::vector<T>& fib_seq() {
        PHAM_STATIC_ASSERT((
               boost::is_same<T, nuwen::uc_t >::value
            || boost::is_same<T, nuwen::us_t >::value
            || boost::is_same<T, nuwen::ul_t >::value
            || boost::is_same<T, nuwen::ull_t>::value));

        static std::vector<T> s_fib;

        if (s_fib.empty()) {
            for (T a = 0, b = 1, c = 1; c >= b; a = b, b = c, c = static_cast<T>(a + b)) {
                s_fib.push_back(c);
            }
        }

        return s_fib;
    }

    template <typename T> void fib_encode(const std::vector<T>& fib, nuwen::pack::packed_bits& pb, T n) {
        if (n == 0) {
            throw std::logic_error("LOGIC ERROR: pham::fib_encode() - Cannot encode a zero.");
        }

        nuwen::db_t d(1, 1);

        typename std::vector<T>::const_iterator j = fib.end() - 1;

        while (*j > n) {
            --j;
        }

        while (true) {
            if (n >= *j) {
                n = static_cast<T>(n - *j);
                d.push_front(1);
            } else {
                d.push_front(0);
            }

            if (j == fib.begin()) {
                break;
            } else {
                --j;
            }
        }

        for (nuwen::db_ci_t i = d.begin(); i != d.end(); ++i) {
            pb.push_back(*i);
        }
    }
}

template <typename C> nuwen::vuc_t nuwen::fibonacci_encode(const C& c) {
    typedef typename C::value_type T;

    const std::vector<T>& fib = pham::fib_seq<T>();
    pack::packed_bits pb;

    for (typename C::const_iterator i = c.begin(); i != c.end(); ++i) {
        pham::fib_encode(fib, pb, *i);
    }

    return pb.vuc();
}

template <typename T> std::vector<T> nuwen::fibonacci_decode(const vuc_t& v) {
    const std::vector<T>& fib = pham::fib_seq<T>();

    std::vector<T> ret;
    T n = 0;
    typename std::vector<T>::const_iterator j = fib.begin();

    for (ull_t i = 0; i < v.size() * 8; ++i) {
        if (n != 0 && bit_from_vuc(v, i - 1) == 1 && bit_from_vuc(v, i) == 1) {
            ret.push_back(n);
            n = 0;
            j = fib.begin();
        } else if (j == fib.end()) {
            throw std::runtime_error("RUNTIME ERROR: nuwen::fibonacci_decode() - Codeword contains too many bits.");
        } else if (bit_from_vuc(v, i) == 1) {
            const T t = static_cast<T>(n + *j);

            if (t < n) {
                throw std::runtime_error("RUNTIME ERROR: nuwen::fibonacci_decode() - Codeword overflows T.");
            }

            n = t;
            ++j;
        } else {
            ++j;
        }
    }

    if (n != 0) {
        throw std::runtime_error("RUNTIME ERROR: nuwen::fibonacci_decode() - Padding contains ones.");
    }

    if (j - fib.begin() > 7) {
        throw std::runtime_error("RUNTIME ERROR: nuwen::fibonacci_decode() - Padding contains too many zeros.");
    }

    return ret;
}

#endif // Idempotency
