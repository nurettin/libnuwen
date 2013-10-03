// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_FILE_HH
#define PHAM_FILE_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "gluon.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <cstdio>
    #include <stdexcept>
    #include <string>
    #include <boost/format.hpp>
    #include <boost/shared_ptr.hpp>
    #include <boost/utility.hpp>
#include "external_end.hh"

namespace pham {
    namespace file {
        class proto_file;
    }
}

namespace nuwen {
    namespace file {
        enum output_mode {
            create,
            overwrite,
            append
        };

        inline vuc_t read_file(const std::string& filename);
        inline void write_file(const vuc_t& v, const std::string& filename, output_mode mode = create);

        inline void remove_file(const std::string& filename);

        class input_file {
        public:
            inline explicit input_file(const std::string& filename);

            inline vuc_t read_at_most(vuc_s_t n);
            inline vuc_t read_rest();

            inline void close();

        private:
            boost::shared_ptr<pham::file::proto_file> m_p;
        };

        class output_file {
        public:
            inline explicit output_file(const std::string& filename, output_mode mode = create);

            inline void write(const vuc_t& v);

            inline void close();

        private:
            boost::shared_ptr<pham::file::proto_file> m_p;
        };
    }
}

namespace pham {
    namespace file {
        using namespace std;

        const nuwen::vuc_s_t BLOCK_SIZE = 65536;

        inline void throw_file_error(const string& fxn, const string& msg, const string& filename) {
            throw runtime_error(str(boost::format("RUNTIME ERROR: %1% - %2% (Filename: \"%3%\")") % fxn % msg % filename));
        }

        inline void throw_file_logic_error(const string& fxn, const string& msg, const string& filename) {
            throw logic_error(str(boost::format("LOGIC ERROR: %1% - %2% (Filename: \"%3%\")") % fxn % msg % filename));
        }

        class proto_file : public boost::noncopyable {
        public:
            proto_file(FILE * const p, const string& s) : m_fp(p), m_name(s) {
                if (m_fp == NULL) {
                    throw_file_logic_error("pham::file::proto_file::proto_file()", "Invalid p.", m_name);
                }
            }

            void close() {
                if (m_fp == NULL) {
                    throw_file_logic_error("pham::file::proto_file::close()", "Called twice.", m_name);
                }

                FILE * const p = m_fp;

                m_fp = NULL;

                if (fclose(p) == EOF) {
                    throw_file_error("pham::file::proto_file::close()", "std::fclose() failed.", m_name);
                }
            }

            ~proto_file() {
                if (m_fp) {
                    fclose(m_fp); // Ignore errors.
                }
            }

            FILE * raw() {
                if (m_fp == NULL) {
                    throw_file_logic_error("pham::file::proto_file::raw()", "File already closed.", m_name);
                }

                return m_fp;
            }

            string name() const {
                return m_name;
            }

        private:
            FILE *       m_fp;
            const string m_name;
        };

        inline FILE * open_read_file(const string& filename) {
            FILE * const r = fopen(filename.c_str(), "rb");

            if (r == NULL) {
                throw_file_error("pham::file::open_read_file()", "std::fopen() failed.", filename);
            }

            return r;
        }

        inline FILE * open_write_file(const string& filename, const nuwen::file::output_mode mode) {
            if (mode == nuwen::file::create) {
                FILE * const r = fopen(filename.c_str(), "rb");

                if (r) {
                    if (fclose(r) == EOF) {
                        throw_file_error("pham::file::open_write_file()", "File already exists and std::fclose() failed.", filename);
                    } else {
                        throw_file_error("pham::file::open_write_file()", "File already exists.", filename);
                    }
                }
            }

            FILE * const w = fopen(filename.c_str(), mode == nuwen::file::append ? "ab" : "wb");

            if (w == NULL) {
                throw_file_error("pham::file::open_write_file()", "std::fopen() failed.", filename);
            }

            return w;
        }
    }
}

inline nuwen::vuc_t nuwen::file::read_file(const std::string& filename) {
    input_file f(filename);

    const vuc_t ret = f.read_rest();

    f.close();

    return ret;
}

inline void nuwen::file::write_file(const vuc_t& v, const std::string& filename, const output_mode mode) {
    output_file f(filename, mode);

    f.write(v);

    f.close();
}

inline void nuwen::file::remove_file(const std::string& filename) {
    using namespace std;

    if (remove(filename.c_str()) != 0) {
        pham::file::throw_file_error("nuwen::file::remove_file()", "std::remove() failed.", filename);
    }
}

inline nuwen::file::input_file::input_file(const std::string& filename)
    : m_p(new pham::file::proto_file(pham::file::open_read_file(filename), filename)) { }

inline nuwen::vuc_t nuwen::file::input_file::read_at_most(const vuc_s_t n) {
    using namespace std;

    if (n == 0) {
        pham::file::throw_file_logic_error("nuwen::file::input_file::read_at_most()", "Invalid n.", m_p->name());
    }

    vuc_t ret(n);

    ret.resize(fread(&ret[0], 1, n, m_p->raw()));

    if (ferror(m_p->raw())) {
        pham::file::throw_file_error("nuwen::file::input_file::read_at_most()", "std::fread() failed.", m_p->name());
    }

    return ret;
}

inline nuwen::vuc_t nuwen::file::input_file::read_rest() {
    using namespace std;

    vuc_t ret;

    vuc_t temp(pham::file::BLOCK_SIZE);

    while (true) {
        temp.resize(fread(&temp[0], 1, pham::file::BLOCK_SIZE, m_p->raw()));

        if (ferror(m_p->raw())) {
            pham::file::throw_file_error("nuwen::file::input_file::read_rest()", "std::fread() failed.", m_p->name());
        }

        ret += cat(temp);

        if (temp.size() != pham::file::BLOCK_SIZE) {
            break;
        }
    }

    return ret;
}

inline void nuwen::file::input_file::close() {
    m_p->close();
}

inline nuwen::file::output_file::output_file(const std::string& filename, const output_mode mode)
    : m_p(new pham::file::proto_file(pham::file::open_write_file(filename, mode), filename)) { }

inline void nuwen::file::output_file::write(const vuc_t& v) {
    using namespace std;

    if (v.empty()) {
        return;
    }

    if (fwrite(&v[0], 1, v.size(), m_p->raw()) != v.size()) {
        pham::file::throw_file_error("nuwen::file::output_file::write()", "std::fwrite() failed.", m_p->name());
    }

    if (fflush(m_p->raw()) != 0) {
        pham::file::throw_file_error("nuwen::file::output_file::write()", "std::fflush() failed.", m_p->name());
    }
}

inline void nuwen::file::output_file::close() {
    m_p->close();
}

#endif // Idempotency
