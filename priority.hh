// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_PRIORITY_HH
#define PHAM_PRIORITY_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "external_begin.hh"
    #include <stdexcept>

    #ifdef NUWEN_PLATFORM_WINDOWS
        #include <windows.h>
    #endif

    #ifdef NUWEN_PLATFORM_UNIX
        #include <sys/time.h>
        #include <sys/resource.h>
    #endif
#include "external_end.hh"

namespace nuwen {
    inline void set_priority_idle();
}

inline void nuwen::set_priority_idle() {
    #ifdef NUWEN_PLATFORM_WINDOWS
        if (SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS) == 0) {
            throw std::runtime_error("RUNTIME ERROR: nuwen::set_priority_idle() - SetPriorityClass() failed.");
        }
    #endif

    #ifdef NUWEN_PLATFORM_UNIX
        if (setpriority(PRIO_PROCESS, 0, 19) != 0) {
            throw std::runtime_error("RUNTIME ERROR: nuwen::set_priority_idle() - setpriority() failed.");
        }
    #endif
}

#endif // Idempotency
