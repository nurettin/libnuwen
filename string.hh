// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_STRING_HH
#define PHAM_STRING_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <algorithm>
    #include <cctype>
    #include <iterator>
    #include <sstream>
    #include <string>
    #include <vector>
    #include <boost/regex.hpp>
#include "external_end.hh"

namespace nuwen {
    inline std::string strip_outside_whitespace(const std::string& s);

    inline std::string& find_and_replace_mutate(std::string& s, const std::string& old, const std::string& nu);

    inline std::string find_and_replace_copy(const std::string& s, const std::string& old, const std::string& nu);

    inline std::string upper(const std::string& s);
    inline std::string lower(const std::string& s);

    inline std::string comma_from_ull(ull_t n);
    inline std::string comma_from_sll(sll_t n);
}

namespace pham {
    inline std::string low_three_digits_from_ull(nuwen::ull_t n);
}

inline std::string nuwen::strip_outside_whitespace(const std::string& s) {
    static const boost::regex r("\\A\\s*(.*?)\\s*\\z");

    return regex_replace(s, r, "$1");
}

inline std::string& nuwen::find_and_replace_mutate(std::string& s, const std::string& old, const std::string& nu) {
    std::string t = find_and_replace_copy(s, old, nu);

    s.swap(t);

    return s;
}

inline std::string nuwen::find_and_replace_copy(const std::string& s, const std::string& old, const std::string& nu) {
    using namespace std;

    vc_t v;

    string::const_iterator i = s.begin();

    while (static_cast<string::size_type>(s.end() - i) >= old.size()) {
        const string::const_iterator match = search(i, s.end(), old.begin(), old.end());

        if (match == s.end()) {
            break;
        }

        copy(i, match, back_inserter(v));

        copy(nu.begin(), nu.end(), back_inserter(v));

        i = match + old.size();
    }

    copy(i, s.end(), back_inserter(v));

    return string_cast<string>(v);
}

inline std::string nuwen::upper(const std::string& s) {
    using namespace std;

    string ret(s);

    for (string::iterator i = ret.begin(); i != ret.end(); ++i) {
        *i = static_cast<char>(toupper(*i));
    }

    return ret;
}

inline std::string nuwen::lower(const std::string& s) {
    using namespace std;

    string ret(s);

    for (string::iterator i = ret.begin(); i != ret.end(); ++i) {
        *i = static_cast<char>(tolower(*i));
    }

    return ret;
}

inline std::string nuwen::comma_from_ull(ull_t n) {
    std::string s = pham::low_three_digits_from_ull(n);
    n /= 1000;

    while (n != 0) {
        s = pham::low_three_digits_from_ull(n) + "," + s;
        n /= 1000;
    }

    return s;
}

inline std::string nuwen::comma_from_sll(const sll_t n) {
    if (static_cast<ull_t>(n) == 0x8000000000000000ULL) {
        return "-9,223,372,036,854,775,808";
    } else if (n < 0) {
        return "-" + comma_from_ull(-n);
    } else {
        return comma_from_ull(n);
    }
}

inline std::string pham::low_three_digits_from_ull(const nuwen::ull_t n) {
    std::ostringstream oss;

    if (n > 999) {
        oss.width(3);
        oss.fill('0');
    }

    return static_cast<std::ostringstream &>(oss << n % 1000).str();
}

#endif // Idempotency
