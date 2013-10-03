// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_MTF_HH
#define PHAM_MTF_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "typedef.hh"

#include "external_begin.hh"
    #include <algorithm>
    #include <boost/utility.hpp>
#include "external_end.hh"

namespace nuwen {
    inline void   mtf2(vuc_t& v);
    inline void unmtf2(vuc_t& v);
}

namespace pham {
    namespace mtf {
        class state : public boost::noncopyable {
        public:
            state() : m_lastposzero(true) {
                for (int i = 0; i < 256; ++i) {
                    m_l[i] = static_cast<nuwen::uc_t>(i);
                }
            }

            nuwen::uc_t get_pos(const nuwen::uc_t byte) const {
                nuwen::uc_t pos = 0;

                while (m_l[pos] != byte) {
                    ++pos;
                }

                return pos;
            }

            nuwen::uc_t get_byte(const nuwen::uc_t pos) const {
                return m_l[pos];
            }

            void update(const nuwen::uc_t byte, const nuwen::uc_t pos) {
                switch (pos) {
                    case 0:
                        m_lastposzero = true;
                        break;
                    case 1:
                        if (!m_lastposzero) {
                            m_l[1] = m_l[0];
                            m_l[0] = byte;
                        }

                        m_lastposzero = false;
                        break;
                    default:
                        std::copy_backward(m_l + 1, m_l + pos, m_l + pos + 1);
                        m_l[1] = byte;
                        m_lastposzero = false;
                        break;
                }
            }

        private:
            nuwen::uc_t m_l[256];
            bool m_lastposzero;
        };
    }
}

inline void nuwen::mtf2(vuc_t& v) {
    pham::mtf::state s;

    for (vuc_i_t i = v.begin(); i != v.end(); ++i) {
        const uc_t byte = *i;
        const uc_t pos = s.get_pos(byte);

        s.update(byte, pos);

        *i = pos;
    }
}

inline void nuwen::unmtf2(vuc_t& v) {
    pham::mtf::state s;

    for (vuc_i_t i = v.begin(); i != v.end(); ++i) {
        const uc_t pos = *i;
        const uc_t byte = s.get_byte(pos);

        s.update(byte, pos);

        *i = byte;
    }
}

#endif // Idempotency
