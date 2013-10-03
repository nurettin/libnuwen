// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_DAEMON_HH
#define PHAM_DAEMON_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#ifdef NUWEN_PLATFORM_UNIX
    #include "external_begin.hh"
        #include <stdexcept>
        #include <unistd.h>
    #include "external_end.hh"
#endif

namespace nuwen {
    inline void daemonize();
}

inline void nuwen::daemonize() {
    #ifdef NUWEN_PLATFORM_UNIX
        if (daemon(0, 0) != 0) {
            throw std::runtime_error("RUNTIME ERROR: nuwen::daemonize() - daemon() failed.");
        }
    #endif
}

#endif // Idempotency
