// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_RANDOM_HH
#define PHAM_RANDOM_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "clock.hh"
#include "time.hh"
#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <boost/random/mersenne_twister.hpp>
    #include <boost/utility.hpp>
#include "external_end.hh"

namespace nuwen {
    namespace random {
        class twister : public boost::noncopyable {
        public:
            inline twister();

            inline uc_t   random_uc();
            inline us_t   random_us();
            inline ul_t   random_ul();
            inline ull_t  random_ull();
            inline float  random_float_0_1();
            inline double random_double_0_1();

        private:
            boost::mt19937 m_mt;
        };
    }
}

namespace pham {
    inline nuwen::ul_t make_seed() {
        using namespace nuwen;

        const ull_t ctr = clock_ctr();

        const ul_t a = static_cast<ul_t>(ctr >> 32);
        const ul_t b = static_cast<ul_t>(ctr & 0xFFFFFFFFUL);

        const ull_t uqh = ull_from_sll(current_qh_time());

        const ul_t c = static_cast<ul_t>(uqh >> 32);
        const ul_t d = static_cast<ul_t>(uqh & 0xFFFFFFFFUL);

        return a ^ b ^ c ^ d;
    }
}

inline nuwen::random::twister::twister() : m_mt(pham::make_seed()) { }

inline nuwen::uc_t nuwen::random::twister::random_uc() {
    return static_cast<nuwen::uc_t>(m_mt() & 0xFF);
}

inline nuwen::us_t nuwen::random::twister::random_us() {
    return static_cast<nuwen::us_t>(m_mt() & 0xFFFF);
}

inline nuwen::ul_t nuwen::random::twister::random_ul() {
    return m_mt();
}

inline nuwen::ull_t nuwen::random::twister::random_ull() {
    ull_t ret = m_mt();
    ret <<= 32;
    ret |= m_mt();

    return ret;
}

inline float nuwen::random::twister::random_float_0_1() {
    return static_cast<float>(random_ul()) / static_cast<float>(0xFFFFFFFFUL);
}

inline double nuwen::random::twister::random_double_0_1() {
    return static_cast<double>(random_ull()) / static_cast<double>(0xFFFFFFFFFFFFFFFFULL);
}

#endif // Idempotency
