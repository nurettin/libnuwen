// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_SHA256_HH
#define PHAM_SHA256_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "gluon.hh"
#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <stdexcept>
#include "external_end.hh"

namespace nuwen {
    inline vuc_t sha256(const vuc_t& v);
}

namespace pham {
    namespace helper256 {
        typedef nuwen::ul_t word_t;

        inline word_t rotr(const word_t x, const int n) {
            return (x >> n) | (x << (32 - n));
        }

        inline word_t ch(const word_t x, const word_t y, const word_t z) {
            return (x & y) ^ (~x & z);
        }

        inline word_t maj(const word_t x, const word_t y, const word_t z) {
            return (x & y) ^ (x & z) ^ (y & z);
        }

        inline word_t big_zero(const word_t x) {
            return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
        }

        inline word_t big_one(const word_t x) {
            return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
        }

        inline word_t small_zero(const word_t x) {
            return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
        }

        inline word_t small_one(const word_t x) {
            return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
        }

        const word_t K[] = {
            0x428A2F98UL, 0x71374491UL, 0xB5C0FBCFUL, 0xE9B5DBA5UL, 0x3956C25BUL, 0x59F111F1UL,
            0x923F82A4UL, 0xAB1C5ED5UL, 0xD807AA98UL, 0x12835B01UL, 0x243185BEUL, 0x550C7DC3UL,
            0x72BE5D74UL, 0x80DEB1FEUL, 0x9BDC06A7UL, 0xC19BF174UL, 0xE49B69C1UL, 0xEFBE4786UL,
            0x0FC19DC6UL, 0x240CA1CCUL, 0x2DE92C6FUL, 0x4A7484AAUL, 0x5CB0A9DCUL, 0x76F988DAUL,
            0x983E5152UL, 0xA831C66DUL, 0xB00327C8UL, 0xBF597FC7UL, 0xC6E00BF3UL, 0xD5A79147UL,
            0x06CA6351UL, 0x14292967UL, 0x27B70A85UL, 0x2E1B2138UL, 0x4D2C6DFCUL, 0x53380D13UL,
            0x650A7354UL, 0x766A0ABBUL, 0x81C2C92EUL, 0x92722C85UL, 0xA2BFE8A1UL, 0xA81A664BUL,
            0xC24B8B70UL, 0xC76C51A3UL, 0xD192E819UL, 0xD6990624UL, 0xF40E3585UL, 0x106AA070UL,
            0x19A4C116UL, 0x1E376C08UL, 0x2748774CUL, 0x34B0BCB5UL, 0x391C0CB3UL, 0x4ED8AA4AUL,
            0x5B9CCA4FUL, 0x682E6FF3UL, 0x748F82EEUL, 0x78A5636FUL, 0x84C87814UL, 0x8CC70208UL,
            0x90BEFFFAUL, 0xA4506CEBUL, 0xBEF9A3F7UL, 0xC67178F2UL };
    }
}

inline nuwen::vuc_t nuwen::sha256(const vuc_t& v) {
    using namespace pham::helper256;

    word_t H[] = {
        0x6A09E667UL, 0xBB67AE85UL, 0x3C6EF372UL, 0xA54FF53AUL,
        0x510E527FUL, 0x9B05688CUL, 0x1F83D9ABUL, 0x5BE0CD19UL };

    vuc_ci_t it  = v.begin();
    vuc_ci_t end = v.end();

    vuc_t padding;

    while (padding.empty() || it != end) {
        if (end - it < 64) {
            if (!padding.empty()) {
                throw std::logic_error("LOGIC ERROR: nuwen::sha256() - padding should be consumed cleanly.");
            }

            const int k = static_cast<int>(63 - (v.size() + 8) % 64);

            padding = vec(cat(vuc_t(it, end))(0x80)(vuc_t(k, 0x00))(vuc_from_ull(v.size() * 8ULL)));

            it  = padding.begin();
            end = padding.end();
        }

        word_t W[64];

        for (int i = 0; i < 16; ++i) {
            W[i] = ul_from_vuc(it, end);
            it += 4;
        }

        for (int i = 16; i < 64; ++i) {
            W[i] = small_one(W[i - 2]) + W[i - 7] + small_zero(W[i - 15]) + W[i - 16];
        }

        word_t a = H[0], b = H[1], c = H[2], d = H[3], e = H[4], f = H[5], g = H[6], h = H[7];

        for (int i = 0; i < 64; ++i) {
            const word_t t1 = h + big_one(e) + ch(e, f, g) + K[i] + W[i];
            const word_t t2 = big_zero(a) + maj(a, b, c);

            h = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }

        H[0] += a; H[1] += b; H[2] += c; H[3] += d; H[4] += e; H[5] += f; H[6] += g; H[7] += h;
    }

    vuc_t hash;
    hash.reserve(32);

    for (int i = 0; i < 8; ++i) {
        hash += cat(vuc_from_ul(H[i]));
    }

    return hash;
}

#endif // Idempotency
