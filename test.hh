// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_TEST_HH
#define PHAM_TEST_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "color.hh"
#include "static_assert_private.hh"

#include "external_begin.hh"
    #include <cstdlib>
    #include <exception>
    #include <iostream>
    #include <ostream>
    #include <set>
    #include <stdexcept>
    #include <string>
#include "external_end.hh"

#define NUWEN_TEST(PHAM_ID, PHAM_WHAT)                           \
    pham::begin_test(PHAM_ID);                                   \
                                                                 \
    try {                                                        \
        if (!(PHAM_WHAT)) {                                      \
            pham::die(PHAM_ID, "returned false.");               \
        }                                                        \
    } catch (const std::logic_error& e) {                        \
        pham::die(PHAM_ID, "failed with std::logic_error", e);   \
    } catch (const std::runtime_error& e) {                      \
        pham::die(PHAM_ID, "failed with std::runtime_error", e); \
    } catch (const std::exception& e) {                          \
        pham::die(PHAM_ID, "failed with std::exception", e);     \
    } catch (...) {                                              \
        pham::die(PHAM_ID, "failed with an unknown exception."); \
    }                                                            \
                                                                 \
    pham::statik<0>::set_message("All tests succeeded.");

namespace pham {
    // This trick is made possible by 14.5.1.3/1.
    template <int N> class statik {
    public:
        PHAM_STATIC_ASSERT(N == 0);

        static void set_message(const std::string& s) {
            s_message = s;
        }

        static void report_test_results() {
            using namespace std;
            using namespace nuwen;

            (s_message == "All tests succeeded." ? cout << green : cout << red) << s_message << gray << endl;
        }

    private:
        static std::string s_message;
    };

    template <int N> std::string statik<N>::s_message;

    inline void set_failure_message(const std::string& id, const std::string& msg) {
        statik<0>::set_message("FAILURE: Test \"" + id + "\" " + msg);
    }

    inline void set_failure_message(const std::string& id, const std::string& msg, const std::exception& e) {
        set_failure_message(id, msg + " \"" + e.what() + "\".");
    }

    inline void die(const std::string& id, const std::string& msg) {
        set_failure_message(id, msg);
        std::exit(EXIT_FAILURE);
    }

    inline void die(const std::string& id, const std::string& msg, const std::exception& e) {
        set_failure_message(id, msg, e);
        std::exit(EXIT_FAILURE);
    }

    inline void begin_test(const std::string& id) {
        using namespace std;

        set_failure_message(id, "exited unexpectedly.");

        static bool s_registered = false;

        if (!s_registered) {
            s_registered = true;

            if (atexit(statik<0>::report_test_results) != 0) {
                throw runtime_error("RUNTIME ERROR: pham::begin_test() - std::atexit() failed.");
            }
        }

        static set<string> s_ids;

        if (!s_ids.insert(id).second) {
            die(id, "has a duplicate ID.");
        }
    }
}

#endif // Idempotency
