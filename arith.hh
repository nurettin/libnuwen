// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_ARITH_HH
#define PHAM_ARITH_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

// Based on the ANSI C Arithmetic Coding Library by Fred Wheeler,
// which was adapted from the program in "Arithmetic Coding For Data
// Compression", by Ian H. Witten, Radford M. Neal, and John G. Cleary,
// Communications Of The ACM, June 1987, Vol. 30, No. 6.

#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <boost/utility.hpp>
#include "external_end.hh"

namespace nuwen {
    inline vuc_t   arith(const vuc_t& v);
    inline vuc_t unarith(const vuc_t& v);
}

namespace pham {
    namespace arith {

// The triple of (CODE_VALUE_BITS, MAX_FREQUENCY, INCREMENT_VALUE) is deeply magic.
// It controls how much compression is achieved by BWT/MTF-2/ZLE/Arith.
// The CACM paper explains that when frequency counts are represented in F bits,
// code values are represented in C bits, and arithmetic is performed with
// P bits of precision, the following must hold: F <= C - 2 and F + C <= P.
// For sl_t arithmetic, P = 31, so F = 14 and C = 16 are possible.
// For ul_t arithmetic, P = 32, so F = 15 and C = 17 are possible.
// CODE_VALUE_BITS is C, while MAX_FREQUENCY may be as large as 2^F - 1.
// As MAX_FREQUENCY becomes larger, the arithmetic coder gains a more precise
// memory (its model of the frequencies becomes closer to the true, unscaled
// frequencies), but the arithmetic coder also becomes less adaptive, as
// rescalings are forced less frequently. Adaptiveness is good for compressing
// BWT/MTF-2/ZLE data, which consists of lengthy stretches of "boring" low bytes,
// interspersed with bursts of "exciting" high bytes. When the arithmetic coder
// adapts quickly, better compression is achieved. A generalization of the
// CACM model can achieve the best of both worlds: use the largest possible
// MAX_FREQUENCY to give the coder a good memory, but increment the frequency counts
// by more than 1. This forces more frequent rescaling.
// (16, 16383, 1): Used by the CACM code (as well as Fred Wheeler's). As they used
// sl_t arithmetic, this was "maximally" precise but not very adaptive.
// (16, 8192, 16): Suggested by Fenwick, this forces rescaling 32x more often,
// at the cost of precision.
// (16, 16383, 32): Used by the first libnuwen implementation, 1.0.22.0. Combines
// Fenwick's rescaling with CACM's precision.
// (17, 32767, 1): Used by libnuwen 1.0.23.0 through 1.0.28.0. I reread the CACM
// paper and realized that ul_t arithmetic allowed higher precision.
// (17, 32767, 64): Used by libnuwen 2.0.0.0 and beyond. Combines maximum precision
// with Fenwick's frequent rescaling.

// Here is how BWT/MTF-2/ZLE/Arith behaves on suall10.txt (9873495 bytes)
// with the various triples:
//     CACM (16, 16383,  1): 2073812 bytes, 1.680 bits/byte: The untuned baseline.
//  Fenwick (16,  8192, 16): 2050015 bytes, 1.661 bits/byte: A clear improvement.
// 1.0.22.0 (16, 16383, 32): 2033592 bytes, 1.648 bits/byte: Even better.
// 1.0.23.0 (17, 32767,  1): 2078591 bytes, 1.684 bits/byte: Crippling regression.
//  2.0.0.0 (17, 32767, 64): 2026462 bytes, 1.642 bits/byte: Best of all worlds.

        const nuwen::ul_t CODE_VALUE_BITS = 17;
        const nuwen::ul_t MAX_FREQUENCY   = 32767;
        const nuwen::ul_t INCREMENT_VALUE = 64;

        const nuwen::ul_t TOP_VALUE       = (1 << CODE_VALUE_BITS) - 1;
        const nuwen::ul_t FIRST_QTR       = TOP_VALUE / 4 + 1;
        const nuwen::ul_t HALF            = 2 * FIRST_QTR;
        const nuwen::ul_t THIRD_QTR       = 3 * FIRST_QTR;

        typedef nuwen::us_t symbol_t;

        const symbol_t    SENTINEL        = 256;
        const symbol_t    NUM_SYMBOLS     = 257;

        class model : public boost::noncopyable {
        public:
            model() {
                for (nuwen::ul_t i = 0; i < NUM_SYMBOLS; ++i) {
                    m_freq[i] = 1;
                }

                for (nuwen::ul_t i = 0; i < NUM_SYMBOLS + 1; ++i) {
                    m_cfreq[i] = NUM_SYMBOLS - i;
                }
            }

            nuwen::ul_t operator[](const symbol_t i) const {
                return m_cfreq[i];
            }

            void update(const symbol_t sym) {
                // The CACM code used a hardcoded increment value of 1 and wrote this test
                // (using current terminology) as m_cfreq[0] == MAX_FREQUENCY, which was correct.
                // libnuwen 1.0.28.0 and earlier used a generalized INCREMENT_VALUE and wrote this test
                // (using current terminology) as m_cfreq[0] >= MAX_FREQUENCY, which was WRONG.
                // The correct generalization is below; an invariant of the model is that
                // m_cfreq[0] is ALWAYS <= MAX_FREQUENCY. If we detect that adding INCREMENT_VALUE
                // will destroy this invariant, we must rescale so that the invariant will be
                // maintained.

                if (m_cfreq[0] + INCREMENT_VALUE > MAX_FREQUENCY) {
                    nuwen::ul_t cum = 0;
                    m_cfreq[NUM_SYMBOLS] = 0;

                    for (int i = NUM_SYMBOLS - 1; i >= 0; --i) {
                        m_freq[i] = (m_freq[i] + 1) / 2;
                        cum += m_freq[i];
                        m_cfreq[i] = cum;
                    }
                }

                m_freq[sym] += INCREMENT_VALUE;

                for (int i = 0; i < sym + 1; ++i) {
                    m_cfreq[i] += INCREMENT_VALUE;
                }
            }

        private:
            nuwen::ul_t m_freq[NUM_SYMBOLS];
            nuwen::ul_t m_cfreq[NUM_SYMBOLS + 1];
        };

        class encoder : public boost::noncopyable {
        public:
            encoder() : m_low(0), m_high(TOP_VALUE), m_fbits(0), m_out(), m_acm() { }

            void encode(const symbol_t sym) {
                const nuwen::ul_t range = m_high - m_low + 1;
                m_high = m_low + range * m_acm[sym] / m_acm[0] - 1;
                m_low  = m_low + range * m_acm[static_cast<symbol_t>(sym + 1)] / m_acm[0];

                while (true) {
                    if (m_high < HALF) {
                        bit_plus_follow(0);
                    } else if (m_low >= HALF) {
                        bit_plus_follow(1);
                        m_low  -= HALF;
                        m_high -= HALF;
                    } else if (m_low >= FIRST_QTR && m_high < THIRD_QTR) {
                        ++m_fbits;
                        m_low  -= FIRST_QTR;
                        m_high -= FIRST_QTR;
                    } else {
                        break;
                    }

                    m_low  = 2 * m_low;
                    m_high = 2 * m_high + 1;
                }

                m_acm.update(sym);
            }

            nuwen::vuc_t finalize() {
                ++m_fbits;

                bit_plus_follow(m_low >= FIRST_QTR);

                return m_out.vuc();
            }

        private:
            void bit_plus_follow(const bool bit) {
                m_out.push_back(bit);

                while (m_fbits > 0) {
                    m_out.push_back(!bit);
                    --m_fbits;
                }
            }

            nuwen::ul_t              m_low;
            nuwen::ul_t              m_high;
            nuwen::ul_t              m_fbits;
            nuwen::pack::packed_bits m_out;
            model                    m_acm;
        };

        class decoder : public boost::noncopyable {
        public:
            decoder(const nuwen::vuc_ci_t start, const nuwen::vuc_ci_t finish)
                : m_curr(start), m_shift(7), m_end(finish), m_value(0), m_low(0), m_high(TOP_VALUE), m_acm() {

                for (nuwen::ul_t i = 0; i < CODE_VALUE_BITS; ++i) {
                    m_value <<= 1;
                    m_value += input_bit();
                }
            }

            symbol_t decode() {
                const nuwen::ul_t range = m_high - m_low + 1;
                const nuwen::ul_t cum = ((m_value - m_low + 1) * m_acm[0] - 1) / range;

                symbol_t sym;

                for (sym = 0; m_acm[static_cast<symbol_t>(sym + 1)] > cum; ++sym) { }

                m_high = m_low + range * m_acm[sym] / m_acm[0] - 1;
                m_low  = m_low + range * m_acm[static_cast<symbol_t>(sym + 1)] / m_acm[0];

                while (true) {
                    if (m_high < HALF) {
                    } else if (m_low >= HALF) {
                        m_value -= HALF;
                        m_low   -= HALF;
                        m_high  -= HALF;
                    } else if (m_low >= FIRST_QTR && m_high < THIRD_QTR) {
                        m_value -= FIRST_QTR;
                        m_low   -= FIRST_QTR;
                        m_high  -= FIRST_QTR;
                    } else {
                        break;
                    }

                    m_low   = 2 * m_low;
                    m_high  = 2 * m_high + 1;
                    m_value = 2 * m_value + input_bit();
                }

                m_acm.update(sym);

                return sym;
            }

        private:
            bool input_bit() {
                if (m_curr != m_end) {
                    const bool ret = *m_curr >> m_shift & 1;

                    if (m_shift == 0) {
                        m_shift = 7;
                        ++m_curr;
                    } else {
                        --m_shift;
                    }

                    return ret;
                } else {
                    return 0;
                }
            }

            nuwen::vuc_ci_t m_curr;
            nuwen::uc_t     m_shift;
            nuwen::vuc_ci_t m_end;
            nuwen::ul_t     m_value;
            nuwen::ul_t     m_low;
            nuwen::ul_t     m_high;
            model           m_acm;
        };
    }
}

inline nuwen::vuc_t nuwen::arith(const vuc_t& v) {
    pham::arith::encoder ae;

    for (vuc_ci_t i = v.begin(); i != v.end(); ++i) {
        ae.encode(*i);
    }

    ae.encode(pham::arith::SENTINEL);

    return ae.finalize();
}

inline nuwen::vuc_t nuwen::unarith(const vuc_t& v) {
    pham::arith::decoder ad(v.begin(), v.end());

    vuc_t ret;

    while (true) {
        const pham::arith::symbol_t decoded = ad.decode();

        if (decoded != pham::arith::SENTINEL) {
            ret.push_back(static_cast<uc_t>(decoded));
        } else {
            return ret;
        }
    }
}

#endif // Idempotency
