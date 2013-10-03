// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_ZLE_HH
#define PHAM_ZLE_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "bwt.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <stdexcept>
#include "external_end.hh"

namespace nuwen {
    inline vuc_t zle(const vuc_t& v);
    inline vuc_t unzle(const vuc_t& v);
}

namespace pham {
    inline void encode_zero_run(nuwen::vuc_t& v, nuwen::ul_t& n) {
        if (n != 0) {
            ++n;

            while (n != 1) {
                v.push_back(static_cast<nuwen::uc_t>(n & 1));
                n >>= 1;
            }

            n = 0;
        }
    }

    inline void decode_zero_run(nuwen::vuc_t& v, nuwen::ul_t& len, nuwen::ul_t& nextbit) {
        if (nextbit != 1) {
            const nuwen::ull_t total_size = static_cast<nuwen::ull_t>(v.size()) + (len | nextbit) - 1;

            if (total_size > 9 + ukk::MAX_ALLOWED_SIZE) {
                throw std::runtime_error("RUNTIME ERROR: pham::decode_zero_run() - Too many bytes produced.");
            }

            v.resize(static_cast<nuwen::vuc_s_t>(total_size), 0x00);

            len = 0;
            nextbit = 1;
        }
    }
}

inline nuwen::vuc_t nuwen::zle(const vuc_t& v) {
    if (v.size() > 9 + pham::ukk::MAX_ALLOWED_SIZE) {
        throw std::logic_error("LOGIC ERROR: nuwen::zle() - v is too big.");
    }

    vuc_t ret;

    ul_t len = 0;

    for (vuc_ci_t i = v.begin(); i != v.end(); ++i) {
        const uc_t byte = *i;

        switch (byte) {
            case 0x00:
                ++len;
                break;

            case 0xFE:
                pham::encode_zero_run(ret, len);
                ret.push_back(0xFF);
                ret.push_back(0x00);
                break;

            case 0xFF:
                pham::encode_zero_run(ret, len);
                ret.push_back(0xFF);
                ret.push_back(0x01);
                break;

            default:
                pham::encode_zero_run(ret, len);
                ret.push_back(static_cast<uc_t>(byte + 1));
                break;
        }
    }

    pham::encode_zero_run(ret, len);

    return ret;
}

inline nuwen::vuc_t nuwen::unzle(const vuc_t& v) {
    vuc_t ret;

    ul_t len = 0;
    ul_t nextbit = 1;

    for (vuc_ci_t i = v.begin(); i != v.end(); ++i) {
        const uc_t byte = *i;

        switch (byte) {
            case 0x00:
                nextbit <<= 1;

                if (nextbit == 0) {
                    throw std::runtime_error("RUNTIME ERROR: nuwen::unzle() - A 0x00 byte overflowed nextbit.");
                }

                break;

            case 0x01:
                len |= nextbit;
                nextbit <<= 1;

                if (nextbit == 0) {
                    throw std::runtime_error("RUNTIME ERROR: nuwen::unzle() - A 0x01 byte overflowed nextbit.");
                }

                break;

            case 0xFF:
                pham::decode_zero_run(ret, len, nextbit);

                if (++i == v.end()) {
                    throw std::runtime_error("RUNTIME ERROR: nuwen::unzle() - A 0xFF byte was followed by nothing.");
                }

                switch (*i) {
                    case 0x00:
                        ret.push_back(0xFE);
                        break;
                    case 0x01:
                        ret.push_back(0xFF);
                        break;
                    default:
                        throw std::runtime_error("RUNTIME ERROR: nuwen::unzle() - A 0xFF byte was followed by an invalid byte.");
                }

                break;

            default:
                pham::decode_zero_run(ret, len, nextbit);

                ret.push_back(static_cast<uc_t>(byte - 1));
                break;
        }
    }

    pham::decode_zero_run(ret, len, nextbit);

    return ret;
}

#endif // Idempotency
