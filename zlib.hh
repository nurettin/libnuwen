// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_ZLIB_HH
#define PHAM_ZLIB_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "gluon.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <stdexcept>
    #include <boost/utility.hpp>
    #include <zlib.h>
#include "external_end.hh"

namespace nuwen {
    inline vuc_t   zlib(const vuc_t& v, int level = Z_BEST_COMPRESSION, int strategy = Z_DEFAULT_STRATEGY);
    inline vuc_t unzlib(const vuc_t& v);
}

namespace pham {
    namespace zlib {
        struct stream : public boost::noncopyable {
            stream(const nuwen::uc_t * const in_ptr, const nuwen::vuc_s_t in_len,
                nuwen::uc_t * const out_ptr, const nuwen::vuc_s_t out_len, const int level, const int strategy) {

                init_common(in_ptr, in_len, out_ptr, out_len);

                if (deflateInit2(&m_stream, level, Z_DEFLATED, 15, 9, strategy) != Z_OK) {
                    throw std::runtime_error("RUNTIME ERROR: pham::zlib::stream::stream() - deflateInit2() failed.");
                }

                m_end = deflateEnd;
            }

            explicit stream(const nuwen::vuc_t& compressed) {
                init_common(&compressed[0], compressed.size(), NULL, 0);

                if (inflateInit(&m_stream) != Z_OK) {
                    throw std::runtime_error("RUNTIME ERROR: pham::zlib::stream::stream() - inflateInit() failed.");
                }

                m_end = inflateEnd;
            }

            void init_common(const nuwen::uc_t * const in_ptr, const nuwen::vuc_s_t in_len,
                nuwen::uc_t * const out_ptr, const nuwen::vuc_s_t out_len) {

                m_stream.next_in   = const_cast<nuwen::uc_t *>(in_ptr);
                m_stream.avail_in  = in_len;
                m_stream.total_in  = 0;
                m_stream.next_out  = out_ptr;
                m_stream.avail_out = out_len;
                m_stream.total_out = 0;
                m_stream.msg       = NULL;
                m_stream.state     = NULL;
                m_stream.zalloc    = NULL;
                m_stream.zfree     = NULL;
                m_stream.opaque    = NULL;
                m_stream.data_type = Z_BINARY;
                m_stream.adler     = 0;
                m_stream.reserved  = 0;
            }

            ~stream() {
                (void) m_end(&m_stream);
            }

            z_stream m_stream;
            int (*m_end)(z_stream *);
        };
    }
}

inline nuwen::vuc_t nuwen::zlib(const vuc_t& v, const int level, const int strategy) {
    const uc_t dummy = 0;

    vuc_t dest(v.size() + v.size() / 1000 + 12);

    pham::zlib::stream s(v.empty() ? &dummy : &v[0], v.size(), &dest[0], dest.size(), level, strategy);

    if (deflate(&s.m_stream, Z_FINISH) != Z_STREAM_END) {
        throw std::runtime_error("RUNTIME ERROR: nuwen::zlib() - deflate() failed.");
    }

    dest.resize(s.m_stream.total_out);

    return dest;
}

inline nuwen::vuc_t nuwen::unzlib(const vuc_t& v) {
    if (v.empty()) {
        throw std::logic_error("LOGIC ERROR: nuwen::unzlib() - v is empty.");
    }

    pham::zlib::stream s(v);

    const vuc_s_t BLOCK_SIZE = 1048576;

    vuc_t temp(BLOCK_SIZE);

    vuc_t dest;

    while (true) {
        s.m_stream.next_out = &temp[0];
        s.m_stream.avail_out = BLOCK_SIZE;

        const int error = inflate(&s.m_stream, Z_SYNC_FLUSH);

        if (error == Z_OK) {
            if (s.m_stream.avail_out != 0) {
                throw std::runtime_error("RUNTIME ERROR: nuwen::unzlib() - Compressed data ended prematurely.");
            }

            dest += cat(temp);
        } else if (error == Z_STREAM_END) {
            if (s.m_stream.avail_in != 0) {
                throw std::runtime_error("RUNTIME ERROR: nuwen::unzlib() - Some bytes were not consumed.");
            }

            temp.resize(BLOCK_SIZE - s.m_stream.avail_out);

            dest += cat(temp);

            return dest;
        } else {
            throw std::runtime_error("RUNTIME ERROR: nuwen::unzlib() - inflate() failed.");
        }
    }
}

#endif // Idempotency
