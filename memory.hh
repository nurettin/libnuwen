// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_MEMORY_HH
#define PHAM_MEMORY_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "typedef.hh"

#ifdef NUWEN_PLATFORM_UNIX
    #include "file.hh"
    #include "vector.hh"
#endif

#include "external_begin.hh"
    #include <stdexcept>

    #ifdef NUWEN_PLATFORM_WINDOWS
        #include <windows.h>
        #include <psapi.h>
    #endif

    #ifdef NUWEN_PLATFORM_UNIX
        #include <string>
        #include <boost/format.hpp>
        #include <boost/lexical_cast.hpp>
        #include <boost/regex.hpp>
        #include <sys/types.h>
        #include <unistd.h>
    #endif
#include "external_end.hh"

namespace nuwen {
    inline ull_t vm_bytes();
}

inline nuwen::ull_t nuwen::vm_bytes() {
    using namespace std;

    #ifdef NUWEN_PLATFORM_WINDOWS
        #ifdef NUWEN_PLATFORM_MINGW
            // Argh! w32api doesn't define PROCESS_MEMORY_COUNTERS_EX.
            // This was taken from public MSDN documentation.

            struct PROCESS_MEMORY_COUNTERS_EX {
                DWORD cb;
                DWORD PageFaultCount;
                SIZE_T PeakWorkingSetSize;
                SIZE_T WorkingSetSize;
                SIZE_T QuotaPeakPagedPoolUsage;
                SIZE_T QuotaPagedPoolUsage;
                SIZE_T QuotaPeakNonPagedPoolUsage;
                SIZE_T QuotaNonPagedPoolUsage;
                SIZE_T PagefileUsage;
                SIZE_T PeakPagefileUsage;
                SIZE_T PrivateUsage;
            };
        #endif

        PROCESS_MEMORY_COUNTERS_EX x;

        x.cb = sizeof x;

        if (GetProcessMemoryInfo(GetCurrentProcess(),
            static_cast<PROCESS_MEMORY_COUNTERS *>(static_cast<void *>(&x)), sizeof x) == 0) {

            throw runtime_error("RUNTIME ERROR: nuwen::vm_bytes() - GetProcessMemoryInfo() failed.");
        }

        return static_cast<ull_t>(x.PrivateUsage);
    #endif

    #ifdef NUWEN_PLATFORM_UNIX
        using namespace boost;
        using namespace nuwen::file;

        const string filename = str(format("/proc/%1%/status") % getpid());

        const vuc_t v = read_file(filename);

        const string s = string_cast<string>(v);

        static const regex r("\\A.*VmSize:\\s*(\\d+) kB.*\\z");

        const string kb = regex_replace(s, r, "$1", regex_constants::format_no_copy);

        if (kb.empty()) {
            throw runtime_error("RUNTIME ERROR: nuwen::vm_bytes() - Parsing /proc/<PID>/status failed.");
        }

        return lexical_cast<ull_t>(kb) * 1024;
    #endif
}

#endif // Idempotency
