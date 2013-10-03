// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "algorithm.hh"
#include "cgi.hh"
#include "compiler.hh"
#include "gluon.hh"
#include "serial.hh"
#include "test.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <cstdlib>
    #include <iostream>
    #include <map>
    #include <ostream>
    #include <stdexcept>
    #include <string>
    #include <boost/iterator/counting_iterator.hpp>
#include "external_end.hh"

using namespace std;
using namespace boost;
using namespace nuwen;
using namespace nuwen::cgi;
using namespace nuwen::ser;
using namespace nuwen::des;

bool test_encoding(const string& s, const string& correct) {
    const string encoded = x_www_form_urlencoded_from_str(s);

    return encoded == correct && str_from_x_www_form_urlencoded(encoded) == s;
}

void test_request_helper(const request& req) {
    if (req["nonexistent"] != "") {
        throw runtime_error("RUNTIME ERROR: test_request_helper() - op[] #1");
    }

    if (req["fluffy kitty"] != "really cute") {
        throw runtime_error("RUNTIME ERROR: test_request_helper() - op[] #2");
    }

    if (req["empty_begin"] != "") {
        throw runtime_error("RUNTIME ERROR: test_request_helper() - op[] #3");
    }

    if (req["empty_end"] != "") {
        throw runtime_error("RUNTIME ERROR: test_request_helper() - op[] #4");
    }

    if (req["colors"] != "red" && req["colors"] != "green" && req["colors"] != "blue") {
        throw runtime_error("RUNTIME ERROR: test_request_helper() - op[] #5");
    }


    if (!req.all_values_for("nonexistent").empty()) {
        throw runtime_error("RUNTIME ERROR: test_request_helper() - all_values_for #1");
    }

    if (req.all_values_for("fluffy kitty") != vec(glu<string>("really cute"))) {
        throw runtime_error("RUNTIME ERROR: test_request_helper() - all_values_for #2");
    }

    if (req.all_values_for("empty_begin") != vec(glu<string>(""))) {
        throw runtime_error("RUNTIME ERROR: test_request_helper() - all_values_for #3");
    }

    if (req.all_values_for("empty_end") != vec(glu<string>(""))) {
        throw runtime_error("RUNTIME ERROR: test_request_helper() - all_values_for #4");
    }

    if (universal_sort_copy(req.all_values_for("colors")) != vec(glu<string>("blue")("green")("red"))) {
        throw runtime_error("RUNTIME ERROR: test_request_helper() - all_values_for #5");
    }


    if (!req.all_values_for("").empty()) {
        throw runtime_error("RUNTIME ERROR: test_request_helper() - Spurious field.");
    }
}

bool test_request() {
    const request req("empty_begin=&fluffy+kitty=really+cute&colors=red&colors=green&colors=blue&empty_end=");

    test_request_helper(req);


    const vuc_t v = serial(req).vuc();

    deserial d(v);

    const request dupe(d);

    test_request_helper(dupe);


    return true;
}

bool test_read_cookie() {
    const map<string, string> cookie = read_cookie("empty_begin=; fluffy+kitty=really+cute; ugly+puppy=totally+dumb; empty_end=");

    map<string, string> m;

    m["empty_begin"]  = "";
    m["fluffy kitty"] = "really cute";
    m["ugly puppy"]   = "totally dumb";
    m["empty_end"]    = "";

    return cookie == m;
}

bool test_make_cookie() {
    map<string, string> m;

    m["fluffy kitty"] = "really cute";
    m["ugly puppy"] = "totally dumb";

    return make_cookie(m) ==
        "Set-Cookie: fluffy+kitty=really+cute; path=/; httponly\nSet-Cookie: ugly+puppy=totally+dumb; path=/; httponly"
        && make_cookie(m, true, 1168956733) ==
        "Set-Cookie: fluffy+kitty=really+cute; expires=Sat, 05-Aug-2006 17:08:02 GMT; path=/; httponly\n"
        "Set-Cookie: ugly+puppy=totally+dumb; expires=Sat, 05-Aug-2006 17:08:02 GMT; path=/; httponly"
        && make_cookie(m, true, 1168956733, ".nuwen.net") ==
        "Set-Cookie: fluffy+kitty=really+cute; expires=Sat, 05-Aug-2006 17:08:02 GMT; domain=.nuwen.net; path=/; httponly\n"
        "Set-Cookie: ugly+puppy=totally+dumb; expires=Sat, 05-Aug-2006 17:08:02 GMT; domain=.nuwen.net; path=/; httponly";
}

bool test_get_content() {
    cout << "get_content(): \"" << get_content() << "\"" << endl;

    return true;
}

int main() {
    NUWEN_TEST("cgi1", test_encoding("cute FLUFFY kittens", "cute+FLUFFY+kittens"))
    NUWEN_TEST("cgi2", test_encoding("foo\nbar", "foo%0D%0Abar"))
    NUWEN_TEST("cgi3", test_encoding("1+1=2 & 4+4=8 ; 6+3=9 a%b", "1%2B1%3D2+%26+4%2B4%3D8+%3B+6%2B3%3D9+a%25b"))
    NUWEN_TEST("cgi4", test_encoding("$-_.!*(),'", "$-_.!*(),'"))

    const string all_chars(make_counting_iterator(0), make_counting_iterator(256));

    NUWEN_TEST("cgi5", str_from_x_www_form_urlencoded(x_www_form_urlencoded_from_str(all_chars)) == all_chars)

    NUWEN_TEST("cgi6", get_env_var("NONEXISTENT_VARIABLE") == "")

    #ifdef NUWEN_PLATFORM_WINDOWS
        NUWEN_TEST("cgi7", _putenv("CGI_TEST_VARIABLE=FOOBAR") == 0)
    #endif

    #ifdef NUWEN_PLATFORM_UNIX
        NUWEN_TEST("cgi7", setenv("CGI_TEST_VARIABLE", "FOOBAR", 0) == 0)
    #endif

    NUWEN_TEST("cgi8", get_env_var("CGI_TEST_VARIABLE") == "FOOBAR")

    NUWEN_TEST("cgi9", test_request())

    NUWEN_TEST("cgi10", test_read_cookie())
    NUWEN_TEST("cgi11", test_make_cookie())

    NUWEN_TEST("cgi12", test_get_content())
}
