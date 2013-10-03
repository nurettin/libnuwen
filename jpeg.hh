// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_JPEG_HH
#define PHAM_JPEG_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "typedef.hh"

#include "external_begin.hh"
    #include <stdexcept>
    #include <boost/format.hpp>
    #include <boost/tuple/tuple.hpp>
    #include <boost/utility.hpp>
    #include <jerror.h>
    #include <jpeglib.h>
#include "external_end.hh"

namespace nuwen {
    inline boost::tuple<ul_t, ul_t, vuc_t> decompress_jpeg_insecurely(const vuc_t& input, ul_t max_width = 2048, ul_t max_height = 2048);
}

namespace pham {
    inline void output_message(jpeg_common_struct * const p) {
        if (p->client_data) {
            p->err->format_message(p, static_cast<char *>(p->client_data));
            p->client_data = NULL;
        }
    }

    inline void do_nothing(jpeg_decompress_struct *) { }

    const nuwen::uc_t fake_eoi_marker[] = { 0xFF, JPEG_EOI };

    inline JPEG_boolean fill_input_buffer(jpeg_decompress_struct * const p) {
        // Obnoxiously, WARNMS is a macro that contains an old-style cast.
        p->err->msg_code = JWRN_JPEG_EOF;
        p->err->emit_message(reinterpret_cast<jpeg_common_struct *>(p), -1);

        p->src->next_input_byte = fake_eoi_marker;
        p->src->bytes_in_buffer = 2;

        return JPEG_TRUE;
    }

    inline void skip_input_data(jpeg_decompress_struct * const p, const long n) { // POISON_OK
        if (n > 0) {
            if (static_cast<nuwen::ul_t>(n) < p->src->bytes_in_buffer) {
                p->src->next_input_byte += n;
                p->src->bytes_in_buffer -= n;
            } else {
                p->src->next_input_byte = NULL;
                p->src->bytes_in_buffer = 0;
            }
        }
    }

    namespace jpeg {
        class decompressor_destroyer : public boost::noncopyable {
        public:
            explicit decompressor_destroyer(jpeg_decompress_struct& d) : m_p(&d) { }

            ~decompressor_destroyer() {
                jpeg_destroy_decompress(m_p);
            }

        private:
            jpeg_decompress_struct * const m_p;
        };
    }
}

inline boost::tuple<nuwen::ul_t, nuwen::ul_t, nuwen::vuc_t> nuwen::decompress_jpeg_insecurely(
    const vuc_t& input, const ul_t max_width, const ul_t max_height) {

    using namespace std;
    using namespace boost;
    using namespace pham;
    using namespace pham::jpeg;

    if (input.empty()) {
        throw runtime_error("RUNTIME ERROR: nuwen::decompress_jpeg_insecurely() - Empty input.");
    }

    jpeg_error_mgr error_manager;
    jpeg_decompress_struct decompressor;

    decompressor.err = jpeg_std_error(&error_manager);

    vc_t warning_message(JMSG_LENGTH_MAX, 0);
    decompressor.client_data = &warning_message[0];
    decompressor.err->output_message = output_message;

    // Obnoxiously, jpeg_create_decompress is a macro that contains an old-style cast.
    jpeg_CreateDecompress(&decompressor, JPEG_LIB_VERSION, sizeof decompressor);

    jpeg_source_mgr source_manager;

    source_manager.next_input_byte   = &input[0];
    source_manager.bytes_in_buffer   = input.size();
    source_manager.init_source       = do_nothing;
    source_manager.fill_input_buffer = fill_input_buffer;
    source_manager.skip_input_data   = skip_input_data;
    source_manager.resync_to_restart = jpeg_resync_to_restart; // Default method.
    source_manager.term_source       = do_nothing;

    decompressor.src = &source_manager;

    decompressor_destroyer destroyer(decompressor);

    jpeg_read_header(&decompressor, JPEG_TRUE);

    const ul_t width  = decompressor.image_width;
    const ul_t height = decompressor.image_height;

    if (width == 0) {
        throw runtime_error("RUNTIME ERROR: nuwen::decompress_jpeg_insecurely() - Zero width.");
    }

    if (height == 0) {
        throw runtime_error("RUNTIME ERROR: nuwen::decompress_jpeg_insecurely() - Zero height.");
    }

    if (width > max_width) {
        throw runtime_error("RUNTIME ERROR: nuwen::decompress_jpeg_insecurely() - Huge width.");
    }

    if (height > max_height) {
        throw runtime_error("RUNTIME ERROR: nuwen::decompress_jpeg_insecurely() - Huge height.");
    }

    if (decompressor.num_components != 3) {
        throw runtime_error("RUNTIME ERROR: nuwen::decompress_jpeg_insecurely() - Unexpected number of components.");
    }

    jpeg_start_decompress(&decompressor);

    tuple<ul_t, ul_t, vuc_t> ret(width, height, vuc_t(width * height * decompressor.num_components, 0));

    uc_t * scanptr = &ret.get<2>()[0];

    // No scaling is being used.
    while (decompressor.output_scanline < height) {
        if (jpeg_read_scanlines(&decompressor, &scanptr, 1) != 1) {
            throw runtime_error("RUNTIME ERROR: nuwen::decompress_jpeg_insecurely() - jpeg_read_scanlines() failed.");
        }

        scanptr += width * decompressor.num_components;
    }

    jpeg_finish_decompress(&decompressor);

    if (decompressor.client_data == NULL) {
        throw runtime_error(str(format(
            "RUNTIME ERROR: nuwen::decompress_jpeg_insecurely() - libjpeg warned \"%1%\".") % &warning_message[0]));
    }

    return ret;
}

#endif // Idempotency
