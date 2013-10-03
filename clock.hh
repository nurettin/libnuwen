// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_CLOCK_HH
#define PHAM_CLOCK_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "typedef.hh"

#include "external_begin.hh"
    #include <stdexcept>

    #ifdef NUWEN_PLATFORM_WINDOWS
        #include <exception>
        #include <boost/utility.hpp>
        #include <windows.h>
    #endif

    #ifdef NUWEN_PLATFORM_UNIX
        #include <algorithm>
        #include <cmath>
        #include <time.h>
        #include <sys/time.h>
    #endif
#include "external_end.hh"

namespace nuwen {
    inline ull_t clock_freq();
    inline ull_t clock_ctr();

    inline void snooze(double s);

    namespace chrono {
        class watch {
        public:
            inline watch();

            inline void reset();

            inline double seconds() const;

        private:
            ull_t  m_ctr;
            double m_freq;
        };
    }
}

#ifdef NUWEN_PLATFORM_WINDOWS
    namespace pham {
        namespace hand {
            struct wrapped_handle : public boost::noncopyable {
                explicit wrapped_handle(const HANDLE h) : m_h(h) {
                    if (m_h == NULL) {
                        throw std::runtime_error("RUNTIME ERROR: pham::hand::wrapped_handle::wrapped_handle() - h is NULL.");
                    }
                }

                ~wrapped_handle() {
                    if (CloseHandle(m_h) == 0) {
                        std::terminate();
                    }
                }

                const HANDLE m_h;
            };
        }
    }
#endif

inline nuwen::ull_t nuwen::clock_freq() {
    #ifdef NUWEN_PLATFORM_WINDOWS
        static ull_t s_freq = 0;

        if (s_freq == 0) {
            LARGE_INTEGER l;

            if (QueryPerformanceFrequency(&l) == 0) {
                throw std::runtime_error("RUNTIME ERROR: nuwen::clock_freq() - QueryPerformanceFrequency() failed.");
            }

            s_freq = static_cast<ull_t>(l.QuadPart);

            if (s_freq == 0) {
                throw std::runtime_error("RUNTIME ERROR: nuwen::clock_freq() - Zero frequency.");
            }
        }

        return s_freq;
    #endif

    #ifdef NUWEN_PLATFORM_UNIX
        return 1000000;
    #endif
}

inline nuwen::ull_t nuwen::clock_ctr() {
    #ifdef NUWEN_PLATFORM_WINDOWS
        LARGE_INTEGER l;

        if (QueryPerformanceCounter(&l) == 0) {
            throw std::runtime_error("RUNTIME ERROR: nuwen::clock_ctr() - QueryPerformanceCounter() failed.");
        }

        return static_cast<ull_t>(l.QuadPart);
    #endif

    #ifdef NUWEN_PLATFORM_UNIX
        struct timeval tv;

        if (gettimeofday(&tv, NULL) != 0) {
            throw std::runtime_error("RUNTIME ERROR: nuwen::clock_ctr() - gettimeofday() failed.");
        }

        return static_cast<ull_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
    #endif
}

inline void nuwen::snooze(const double s) {
    using namespace std;

    if (s < .000001) {
        throw logic_error("LOGIC ERROR: nuwen::snooze() - Invalid s.");
    }

    #ifdef NUWEN_PLATFORM_WINDOWS
        pham::hand::wrapped_handle h(CreateWaitableTimer(NULL, TRUE, NULL));

        LARGE_INTEGER due_time;

        due_time.QuadPart = static_cast<LONGLONG>(-10000000 * s);

        if (SetWaitableTimer(h.m_h, &due_time, 0, NULL, NULL, 0) == 0) {
            throw runtime_error("RUNTIME ERROR: nuwen::snooze() - SetWaitableTimer() failed.");
        }

        if (WaitForSingleObject(h.m_h, INFINITE) != WAIT_OBJECT_0) {
            throw runtime_error("RUNTIME ERROR: nuwen::snooze() - WaitForSingleObject() failed.");
        }
    #endif

    #ifdef NUWEN_PLATFORM_UNIX
        timespec t;

        t.tv_sec = static_cast<ull_t>(s);
        t.tv_nsec = min(static_cast<ull_t>((s - floor(s)) * 1000000000), static_cast<ull_t>(999999999));

        if (nanosleep(&t, NULL) != 0) {
            throw runtime_error("RUNTIME ERROR: nuwen::snooze() - nanosleep() failed.");
        }
    #endif
}

inline nuwen::chrono::watch::watch() : m_ctr(clock_ctr()), m_freq(static_cast<double>(clock_freq())) { }

inline void nuwen::chrono::watch::reset() { m_ctr = clock_ctr(); }

inline double nuwen::chrono::watch::seconds() const { return static_cast<double>(clock_ctr() - m_ctr) / m_freq; }

#endif // Idempotency
