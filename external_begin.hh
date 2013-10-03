// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifdef PHAM_IN_EXTERNAL
    #error external_begin.hh included twice.
#endif

#define PHAM_IN_EXTERNAL

#ifdef _WIN32
    #ifdef WINVER
        #if WINVER < 0x0501
            #error libnuwen requires WINVER >= 0x0501 but someone defined it to be lower.
        #endif
    #else
        #define WINVER 0x0501
    #endif

    #ifdef _WIN32_WINNT
        #if _WIN32_WINNT < 0x0501
            #error libnuwen requires _WIN32_WINNT >= 0x0501 but someone defined it to be lower.
        #endif
    #else
        #define _WIN32_WINNT 0x0501
    #endif

    // Prevent the macro in <windef.h> from interfering with the template in <algorithm>.
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    // By default, <windows.h> includes a bunch of other headers that it doesn't actually depend on.
    // This is for "convenience", but it slows down compilation when these other headers aren't needed.
    // Also, on MSVC, <winsock.h> (which conflicts with <winsock2.h>) is one of these other headers.

    // Prevent <windows.h> from including these other headers.
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
#endif

#ifdef _MSC_VER
    // Disable all warnings that are triggered by non-libnuwen headers.
    #pragma warning(push)
    #pragma warning(disable : 4061) // No explicit case label for an enum.
    #pragma warning(disable : 4242) // Narrowing conversion.
    #pragma warning(disable : 4244) // Narrowing conversion.
    #pragma warning(disable : 4365) // Signed/Unsigned mismatch in conversion.
    #pragma warning(disable : 4548) // Expression before comma has no effect.
    #pragma warning(disable : 4555) // Expression has no effect.
    #pragma warning(disable : 4619) // There is no warning number N.
    #pragma warning(disable : 4668) // Replacing non-macro-name identifier with 0 as required by 16.1/4.
    #pragma warning(disable : 4701) // Potentially uninitialized local variable used.
    #pragma warning(disable : 4738) // Storing 32-bit float in memory.
    #pragma warning(disable : 4800) // Conversion to bool.
    #pragma warning(disable : 4826) // Sign-extended conversion.
#endif
