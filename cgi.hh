// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_CGI_HH
#define PHAM_CGI_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "serial.hh"
#include "string.hh"
#include "time.hh"
#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <cstdio>
    #include <cstdlib>
    #include <exception>
    #include <map>
    #include <sstream>
    #include <stdexcept>
    #include <string>
    #include <utility>
    #include <boost/format.hpp>
    #include <boost/lexical_cast.hpp>
#include "external_end.hh"

namespace nuwen {
    // See http://tools.ietf.org/html/rfc1866 .
    inline std::string x_www_form_urlencoded_from_str(const std::string& s);
    inline std::string str_from_x_www_form_urlencoded(const std::string& s);

    inline std::string get_env_var(const char * name);
    inline std::string get_content();

    namespace cgi {
        class request {
        public:
            inline explicit request(const std::string& s);

            inline ser::serial& serialize(ser::serial& s) const;
            inline explicit request(des::deserial& d);

            inline std::string operator[](const std::string& field) const;
            inline vs_t    all_values_for(const std::string& field) const;

        private:
            typedef std::multimap<std::string, std::string> mm_t;
            typedef mm_t::const_iterator mm_ci_t;

            mm_t m_mm;
        };
    }

    inline std::map<std::string, std::string> read_cookie(const std::string& s);

    // If m is empty, a std::logic_error will be thrown.
    // If a string longer than 4096 characters is generated, a std::runtime_error will be thrown.
    // If not persistent, it is a session cookie. The expiration date and domain are ignored.
    // If persistent, an expiration date is necessary. A domain such as ".nuwen.net" is optional.
    //     By default, when the domain is "", cookies are sent back to only the originating server.

    inline std::string make_cookie(const std::map<std::string, std::string>& m, bool persistent = false,
        sll_t absolute_expiration_date = current_qh_time() + 3600, const std::string& domain = "");
}

inline std::string nuwen::x_www_form_urlencoded_from_str(const std::string& s) {
    std::string ret;

    for (std::string::const_iterator i = s.begin(); i != s.end(); ++i) {
        const char c = *i;

        switch (c) {
            case ' ':
                ret += '+';
                break;

            case '\n':
                ret += "%0D%0A";
                break;

            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
            case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
            case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
            case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
            case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            case '$': case '-': case '_': case '.': case '!': case '*': case '(': case ')': case ',': case '\'':
                ret += c;
                break;

            default:
                ret += '%';
                ret += hex_from_uc(static_cast<uc_t>(c));
                break;
        }
    }

    return ret;
}

inline std::string nuwen::str_from_x_www_form_urlencoded(const std::string& s) {
    std::string ret;

    for (std::string::const_iterator i = s.begin(); i != s.end(); ++i) {
        const char c = *i;

        switch (c) {
            case '+':
                ret += ' ';
                break;

            case '%':
                try {
                    ret += uc_from_hex(s.substr(i - s.begin() + 1, 2));
                    i += 2;
                } catch (const std::exception&) {
                    return "";
                }

                break;

            default:
                ret += c;
                break;
        }
    }

    find_and_replace_mutate(ret, "\r\n", "\n");

    return ret;
}

inline std::string nuwen::get_env_var(const char * const name) {
    const char * const e = std::getenv(name);

    return e ? e : "";
}

inline std::string nuwen::get_content() {
    static bool s_inited = false;
    static std::string s_content;

    if (!s_inited) {
        s_inited = true;

        const std::string request_method = get_env_var("REQUEST_METHOD");

        if (request_method == "GET") {
            s_content = get_env_var("QUERY_STRING");
        } else if (request_method == "POST") {
            const ul_t n = boost::lexical_cast<ul_t>(get_env_var("CONTENT_LENGTH"));

            if (n > 0) {
                vuc_t v(n);

                v.resize(std::fread(&v[0], 1, n, stdin));

                s_content = string_cast<std::string>(v);
            }
        }
    }

    return s_content;
}

inline nuwen::cgi::request::request(const std::string& s) {
    std::istringstream iss(s);

    for (std::string field, value; std::getline(iss, field, '='); ) {
        std::getline(iss, value, '&');
        m_mm.insert(std::make_pair(str_from_x_www_form_urlencoded(field), str_from_x_www_form_urlencoded(value)));
    }
}

inline nuwen::ser::serial& nuwen::cgi::request::serialize(ser::serial& s) const {
    return s << m_mm;
}

inline nuwen::cgi::request::request(des::deserial& d)
    : m_mm(d.get<mm_t>()) { }

inline std::string nuwen::cgi::request::operator[](const std::string& field) const {
    const mm_ci_t i = m_mm.find(field);

    return i == m_mm.end() ? "" : i->second;
}

inline nuwen::vs_t nuwen::cgi::request::all_values_for(const std::string& field) const {
    vs_t ret;

    const std::pair<mm_ci_t, mm_ci_t> p = m_mm.equal_range(field);

    for (mm_ci_t i = p.first; i != p.second; ++i) {
        ret.push_back(i->second);
    }

    return ret;
}

inline std::map<std::string, std::string> nuwen::read_cookie(const std::string& s) {
    std::map<std::string, std::string> ret;

    std::istringstream iss(s);

    for (std::string name, value; std::getline(iss, name, '='); ) {
        std::getline(iss, value, ';');

        ret[str_from_x_www_form_urlencoded(strip_outside_whitespace(name))]
            = str_from_x_www_form_urlencoded(strip_outside_whitespace(value));
    }

    return ret;
}

inline std::string nuwen::make_cookie(const std::map<std::string, std::string>& m,
    const bool persistent, const sll_t absolute_expiration_date, const std::string& domain) {

    using namespace std;

    if (m.empty()) {
        throw logic_error("LOGIC ERROR: nuwen::make_cookie() - No data.");
    }

    const string expiration_part = persistent
        ? "; expires=" + format_qh(absolute_expiration_date, 0, false, "DDD, DD-MMM-YYYY HH:mm:ss *GMT*")
        : "";

    const string domain_part = persistent && !domain.empty() ? "; domain=" + domain : "";

    string ret;

    for (map<string, string>::const_iterator i = m.begin(); i != m.end(); ++i) {
        const string cookie = str(boost::format("Set-Cookie: %1%=%2%%3%%4%; path=/; httponly")
            % x_www_form_urlencoded_from_str(i->first)
            % x_www_form_urlencoded_from_str(i->second)
            % expiration_part
            % domain_part);

        if (cookie.size() > 4096) {
            throw runtime_error("RUNTIME ERROR: nuwen::make_cookie() - Cookie is too large.");
        }

        ret += (ret.empty() ? "" : "\n") + cookie;
    }

    return ret;
}

#endif // Idempotency
