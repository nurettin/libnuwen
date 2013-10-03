// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_BZIP2_HH
#define PHAM_BZIP2_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "gluon.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <stdexcept>
    #include <boost/utility.hpp>
    #include <bzlib.h>
#include "external_end.hh"

namespace nuwen {
    inline vuc_t   bzip2(const vuc_t& v);
    inline vuc_t unbzip2(const vuc_t& v);
}

namespace pham {
    namespace bz2 {
        struct stream : public boost::noncopyable {
            explicit stream(const nuwen::vuc_t& v) {
                m_stream.next_in        = NULL;
                m_stream.avail_in       = 0;
                m_stream.total_in_lo32  = 0;
                m_stream.total_in_hi32  = 0;
                m_stream.next_out       = NULL;
                m_stream.avail_out      = 0;
                m_stream.total_out_lo32 = 0;
                m_stream.total_out_hi32 = 0;
                m_stream.state          = NULL;
                m_stream.bzalloc        = NULL;
                m_stream.bzfree         = NULL;
                m_stream.opaque         = NULL;

                if (BZ2_bzDecompressInit(&m_stream, 0, 0) != BZ_OK) {
                    throw std::runtime_error("RUNTIME ERROR: pham::bz2::stream::stream() - BZ2_bzDecompressInit() failed.");
                }

                m_stream.next_in = reinterpret_cast<char *>(const_cast<nuwen::uc_t *>(&v[0]));
                m_stream.avail_in = v.size();
            }

            ~stream() {
                (void) BZ2_bzDecompressEnd(&m_stream);
            }

            bz_stream m_stream;
        };
    }
}

inline nuwen::vuc_t nuwen::bzip2(const vuc_t& v) {
    unsigned int destlen = v.size() + v.size() / 100 + 600; // POISON_OK

    vuc_t dest(destlen);

    char dummy = 0;

    char * const source = v.empty() ? &dummy : reinterpret_cast<char *>(const_cast<uc_t *>(&v[0]));

    const int error = BZ2_bzBuffToBuffCompress(reinterpret_cast<char *>(&dest[0]), &destlen, source, v.size(), 9, 0, 0);

    if (error != BZ_OK) {
        throw std::runtime_error("RUNTIME ERROR: nuwen::bzip2() - BZ2_bzBuffToBuffCompress() failed.");
    }

    dest.resize(destlen);
    return dest;
}

inline nuwen::vuc_t nuwen::unbzip2(const vuc_t& v) {
    if (v.empty()) {
        throw std::logic_error("LOGIC ERROR: nuwen::unbzip2() - v is empty.");
    }

    pham::bz2::stream s(v);

    const vuc_s_t BLOCK_SIZE = 1048576;

    vuc_t temp(BLOCK_SIZE);

    vuc_t dest;

    while (true) {
        s.m_stream.next_out = reinterpret_cast<char *>(&temp[0]);
        s.m_stream.avail_out = BLOCK_SIZE;

        const int error = BZ2_bzDecompress(&s.m_stream);

        if (error == BZ_OK) {
            if (s.m_stream.avail_out != 0) {
                throw std::runtime_error("RUNTIME ERROR: nuwen::unbzip2() - Compressed data ended prematurely.");
            }

            dest += cat(temp);
        } else if (error == BZ_STREAM_END) {
            if (s.m_stream.avail_in != 0) {
                throw std::runtime_error("RUNTIME ERROR: nuwen::unbzip2() - Some bytes were not consumed.");
            }

            temp.resize(BLOCK_SIZE - s.m_stream.avail_out);

            dest += cat(temp);

            return dest;
        } else {
            throw std::runtime_error("RUNTIME ERROR: nuwen::unbzip2() - BZ2_bzDecompress() failed.");
        }
    }
}

#endif // Idempotency
