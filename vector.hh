// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_VECTOR_HH
#define PHAM_VECTOR_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "gluon.hh"
#include "static_assert_private.hh"
#include "traits.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <cstddef>
    #include <deque>
    #include <list>
    #include <stdexcept>
    #include <string>
    #include <vector>
    #include <boost/mpl/or.hpp>
    #include <boost/type_traits.hpp>
    #include <boost/utility/enable_if.hpp>
#include "external_end.hh"

namespace nuwen {
    namespace pack {
        class packed_bits {
        public:
            inline packed_bits();
            inline void push_back(bool bit);
            inline vuc_t vuc() const;

        private:
            vuc_t m_v;
            uc_t  m_bits;
            uc_t  m_numbits;
        };
    }

    inline ull_t bytes_from_bits(ull_t n);

    inline bool bit_from_vuc(const vuc_t& v, ull_t n);

    template <typename DstCont, typename SrcCont> DstCont sequence_cast(const SrcCont& s);

    template <typename DstCont, typename SrcCont>
        typename boost::disable_if<boost::mpl::or_<boost::is_array<SrcCont>, boost::is_pointer<SrcCont> >, DstCont>::type
        string_cast(const SrcCont& s);

    template <typename DstCont> DstCont string_cast(const char * c);

    inline  uc_t  uc_from_sc ( sc_t x);
    inline  us_t  us_from_ss ( ss_t x);
    inline  ul_t  ul_from_sl ( sl_t x);
    inline ull_t ull_from_sll(sll_t x);

    inline  sc_t  sc_from_uc ( uc_t x);
    inline  ss_t  ss_from_us ( us_t x);
    inline  sl_t  sl_from_ul ( ul_t x);
    inline sll_t sll_from_ull(ull_t x);

    inline  us_t  us_from_vuc(const vuc_t& v, vuc_s_t n = 0);
    inline  ul_t  ul_from_vuc(const vuc_t& v, vuc_s_t n = 0);
    inline ull_t ull_from_vuc(const vuc_t& v, vuc_s_t n = 0);

    inline  us_t  us_from_vuc(vuc_ci_t i, vuc_ci_t end);
    inline  ul_t  ul_from_vuc(vuc_ci_t i, vuc_ci_t end);
    inline ull_t ull_from_vuc(vuc_ci_t i, vuc_ci_t end);

    inline vuc_t vuc_from_us ( us_t x);
    inline vuc_t vuc_from_ul ( ul_t x);
    inline vuc_t vuc_from_ull(ull_t x);

    inline std::string hex_from_uc(uc_t c);
    inline std::string hex_from_vuc(const vuc_t& v);

    inline uc_t uc_from_hex(const std::string& s, std::string::size_type pos = 0);
    inline vuc_t vuc_from_hex(const std::string& s);
}

namespace pham {
    const nuwen::ull_t big = static_cast<nuwen::ull_t>(1) << 63;
    const nuwen::sll_t tiny = static_cast<nuwen::sll_t>(1) << 62;

    template <typename T> T t_from_vuc(const nuwen::vuc_t& v, nuwen::vuc_s_t n);
    template <typename T> T t_from_vuc(nuwen::vuc_ci_t i, nuwen::vuc_ci_t end);
    template <typename T> T t_from_vuc_unchecked(nuwen::vuc_ci_t i);

    template <typename T> nuwen::vuc_t vuc_from_t(T x);

    inline nuwen::uc_t uc_from_hexit(char c);
}

inline nuwen::pack::packed_bits::packed_bits()
    : m_v(), m_bits(0), m_numbits(0) { }

inline void nuwen::pack::packed_bits::push_back(const bool bit) {
    m_bits = static_cast<uc_t>(m_bits | (bit << (7 - m_numbits++)));

    if (m_numbits == 8) {
        m_v.push_back(m_bits);
        m_bits = 0;
        m_numbits = 0;
    }
}

inline nuwen::vuc_t nuwen::pack::packed_bits::vuc() const {
    return m_numbits == 0 ? m_v : vec(cat(m_v)(m_bits));
}

inline nuwen::ull_t nuwen::bytes_from_bits(const ull_t n) {
    return n / 8 + (n % 8 != 0);
}

inline bool nuwen::bit_from_vuc(const vuc_t& v, const ull_t n) {
    if (n >= static_cast<ull_t>(v.size()) * 8) {
        throw std::logic_error("LOGIC ERROR: nuwen::bit_from_vuc() - Invalid n.");
    }

    return v[static_cast<vuc_s_t>(n / 8)] >> (7 - n % 8) & 1;
}

template <typename DstCont, typename SrcCont> DstCont nuwen::sequence_cast(const SrcCont& s) {
    PHAM_STATIC_ASSERT(!boost::is_const<DstCont>::value);

    PHAM_STATIC_ASSERT((!boost::is_same<DstCont, SrcCont>::value));

    PHAM_STATIC_ASSERT((is_sequence<DstCont>::value && is_sequence<SrcCont>::value
        && boost::is_same<typename DstCont::value_type, typename SrcCont::value_type>::value));

    return DstCont(s.begin(), s.end());
}

template <typename DstCont, typename SrcCont>
    typename boost::disable_if<boost::mpl::or_<boost::is_array<SrcCont>, boost::is_pointer<SrcCont> >, DstCont>::type
    nuwen::string_cast(const SrcCont& s) {

    PHAM_STATIC_ASSERT(!boost::is_const<DstCont>::value);

    PHAM_STATIC_ASSERT((!boost::is_same<DstCont, SrcCont>::value));

    PHAM_STATIC_ASSERT((is_string<DstCont>::value && is_stringlike_sequence<SrcCont>::value)
        || (is_stringlike_sequence<DstCont>::value && is_string<SrcCont>::value));

    return DstCont(s.begin(), s.end());
}

template <typename DstCont> DstCont nuwen::string_cast(const char * const c) {
    return string_cast<DstCont>(std::string(c));
}

inline nuwen::uc_t nuwen::uc_from_sc(const sc_t x) {
    return static_cast<uc_t>(static_cast<ss_t>(x) + static_cast<ss_t>(1 << 7));
}

inline nuwen::us_t nuwen::us_from_ss(const ss_t x) {
    return static_cast<us_t>(static_cast<sl_t>(x) + static_cast<sl_t>(1 << 15));
}

inline nuwen::ul_t nuwen::ul_from_sl(const sl_t x) {
    return static_cast<ul_t>(static_cast<sll_t>(x) + static_cast<sll_t>(1 << 31));
}

inline nuwen::ull_t nuwen::ull_from_sll(const sll_t x) {
    return x >= 0
        ? static_cast<ull_t>(x) + pham::big
        : static_cast<ull_t>(x + pham::tiny + pham::tiny);
}

inline nuwen::sc_t nuwen::sc_from_uc(const uc_t x) {
    return static_cast<sc_t>(static_cast<ss_t>(x) - static_cast<ss_t>(1 << 7));
}

inline nuwen::ss_t nuwen::ss_from_us(const us_t x) {
    return static_cast<ss_t>(static_cast<sl_t>(x) - static_cast<sl_t>(1 << 15));
}

inline nuwen::sl_t nuwen::sl_from_ul(const ul_t x) {
    return static_cast<sl_t>(static_cast<sll_t>(x) - static_cast<sll_t>(1 << 31));
}

inline nuwen::sll_t nuwen::sll_from_ull(const ull_t x) {
    return x >= pham::big
        ? static_cast<sll_t>(x - pham::big)
        : static_cast<sll_t>(x) - pham::tiny - pham::tiny;
}

inline nuwen::us_t nuwen::us_from_vuc(const vuc_t& v, const vuc_s_t n) {
    return pham::t_from_vuc<us_t>(v, n);
}

inline nuwen::ul_t nuwen::ul_from_vuc(const vuc_t& v, const vuc_s_t n) {
    return pham::t_from_vuc<ul_t>(v, n);
}

inline nuwen::ull_t nuwen::ull_from_vuc(const vuc_t& v, const vuc_s_t n) {
    return pham::t_from_vuc<ull_t>(v, n);
}

inline nuwen::us_t nuwen::us_from_vuc(const vuc_ci_t i, const vuc_ci_t end) {
    return pham::t_from_vuc<us_t>(i, end);
}

inline nuwen::ul_t nuwen::ul_from_vuc(const vuc_ci_t i, const vuc_ci_t end) {
    return pham::t_from_vuc<ul_t>(i, end);
}

inline nuwen::ull_t nuwen::ull_from_vuc(const vuc_ci_t i, const vuc_ci_t end) {
    return pham::t_from_vuc<ull_t>(i, end);
}

inline nuwen::vuc_t nuwen::vuc_from_us(const us_t x) {
    return pham::vuc_from_t(x);
}

inline nuwen::vuc_t nuwen::vuc_from_ul(const ul_t x) {
    return pham::vuc_from_t(x);
}

inline nuwen::vuc_t nuwen::vuc_from_ull(const ull_t x) {
    return pham::vuc_from_t(x);
}

inline std::string nuwen::hex_from_uc(const uc_t c) {
    std::string s(2, 'x');

    s[0] = "0123456789ABCDEF"[c >> 4];
    s[1] = "0123456789ABCDEF"[c & 0x0F];

    return s;
}

inline std::string nuwen::hex_from_vuc(const vuc_t& v) {
    std::string s(2 * v.size(), 'x');

    std::string::iterator k = s.begin();

    for (vuc_ci_t i = v.begin(); i != v.end(); ++i) {
        *k++ = "0123456789ABCDEF"[*i >> 4];
        *k++ = "0123456789ABCDEF"[*i & 0x0F];
    }

    return s;
}

inline nuwen::uc_t nuwen::uc_from_hex(const std::string& s, const std::string::size_type pos) {
    if (s.size() - pos < 2) {
        throw std::logic_error("LOGIC ERROR: nuwen::uc_from_hex() - Invalid pos.");
    }

    return static_cast<uc_t>(pham::uc_from_hexit(s[pos]) * 16 + pham::uc_from_hexit(s[pos + 1]));
}

inline nuwen::vuc_t nuwen::vuc_from_hex(const std::string& s) {
    if (s.size() % 2 != 0) {
        throw std::runtime_error("RUNTIME ERROR: nuwen::vuc_from_hex() - s.size() is odd.");
    }

    vuc_t v(s.size() / 2);

    vuc_i_t k = v.begin();

    for (std::string::size_type i = 0; i < s.size(); i += 2) {
        *k++ = static_cast<uc_t>(pham::uc_from_hexit(s[i]) * 16 + pham::uc_from_hexit(s[i + 1]));
    }

    return v;
}

template <typename T> T pham::t_from_vuc(const nuwen::vuc_t& v, const nuwen::vuc_s_t n) {
    if (n + sizeof(T) > v.size()) {
        throw std::logic_error("LOGIC ERROR: pham::t_from_vuc() - Invalid n.");
    }

    return t_from_vuc_unchecked<T>(v.begin() + static_cast<nuwen::vuc_d_t>(n));
}

template <typename T> T pham::t_from_vuc(const nuwen::vuc_ci_t i, const nuwen::vuc_ci_t end) {
    if (i + sizeof(T) > end) {
        throw std::logic_error("LOGIC ERROR: pham::t_from_vuc() - Invalid i.");
    }

    return t_from_vuc_unchecked<T>(i);
}

template <typename T> T pham::t_from_vuc_unchecked(nuwen::vuc_ci_t i) {
    T t = 0;

    for (std::size_t j = 0; j < sizeof(T); ++j) {
        t = static_cast<T>(t << 8);
        t = static_cast<T>(t | *i++);
    }

    return t;
}

template <typename T> nuwen::vuc_t pham::vuc_from_t(const T x) {
    nuwen::vuc_t v(sizeof(T));

    for (std::size_t i = 0; i < sizeof(T); ++i) {
        v[i] = static_cast<nuwen::uc_t>(x >> (sizeof(T) - 1 - i) * 8 & 0xFF);
    }

    return v;
}

inline nuwen::uc_t pham::uc_from_hexit(const char c) {
    switch (c) {
                  case '0': return 0x0;
                  case '1': return 0x1;
                  case '2': return 0x2;
                  case '3': return 0x3;
                  case '4': return 0x4;
                  case '5': return 0x5;
                  case '6': return 0x6;
                  case '7': return 0x7;
                  case '8': return 0x8;
                  case '9': return 0x9;
        case 'a': case 'A': return 0xA;
        case 'b': case 'B': return 0xB;
        case 'c': case 'C': return 0xC;
        case 'd': case 'D': return 0xD;
        case 'e': case 'E': return 0xE;
        case 'f': case 'F': return 0xF;
        default:
            throw std::runtime_error("RUNTIME ERROR: pham::uc_from_hexit() - Invalid c.");
    }
}

#endif // Idempotency
