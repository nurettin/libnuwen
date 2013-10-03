// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_COLOR_PRIVATE_HH
#define PHAM_COLOR_PRIVATE_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#ifdef NUWEN_PLATFORM_GCC
    #pragma GCC system_header
#endif

#ifdef NUWEN_PLATFORM_WINDOWS
    #include "external_begin.hh"
        #include <windows.h>
    #include "external_end.hh"

    namespace pham {
        const DWORD STD_OUTPUT_HANDLE_CONSTANT = STD_OUTPUT_HANDLE;
        const HANDLE INVALID_HANDLE_VALUE_CONSTANT = INVALID_HANDLE_VALUE;
    }
#endif

#endif // Idempotency
