// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_COMPILER_HH
#define PHAM_COMPILER_HH

#ifdef __GNUC__
    #define NUWEN_PLATFORM_GCC
#endif

#ifdef __MINGW32__
    #define NUWEN_PLATFORM_MINGW
#endif

#ifdef _MSC_VER
    #define NUWEN_PLATFORM_MSVC
#endif

#ifdef _WIN32
    #define NUWEN_PLATFORM_WINDOWS
#else
    #define NUWEN_PLATFORM_UNIX
#endif

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#define NUWEN_MAJOR_VERSION 2
#define NUWEN_MINOR_VERSION 0
#define NUWEN_GIZMO_VERSION 1
#define NUWEN_PATCH_VERSION 2

#define PHAM_STRINGIZE(PHAM_ARG) #PHAM_ARG
#define NUWEN_STRINGIZE(PHAM_ARG) PHAM_STRINGIZE(PHAM_ARG)

#define NUWEN_VERSION                        \
    NUWEN_STRINGIZE(NUWEN_MAJOR_VERSION) "." \
    NUWEN_STRINGIZE(NUWEN_MINOR_VERSION) "." \
    NUWEN_STRINGIZE(NUWEN_GIZMO_VERSION) "." \
    NUWEN_STRINGIZE(NUWEN_PATCH_VERSION)

#ifdef NUWEN_PLATFORM_GCC
    #ifdef NUWEN_PLATFORM_MINGW
        #define NUWEN_COMPILER_NAME "MinGW GCC"
    #else
        #define NUWEN_COMPILER_NAME "GCC"
    #endif

    #define NUWEN_COMPILER_VERSION \
        NUWEN_STRINGIZE(__GNUC__) "." \
        NUWEN_STRINGIZE(__GNUC_MINOR__) "." \
        NUWEN_STRINGIZE(__GNUC_PATCHLEVEL__)
#elif defined NUWEN_PLATFORM_MSVC
    #define NUWEN_COMPILER_NAME "Microsoft Visual Studio"

    #if _MSC_VER == 1400
        #define NUWEN_COMPILER_VERSION "2005 (8.0)"
    #else
        #define NUWEN_COMPILER_VERSION "(unknown version)"
    #endif
#else
    #define NUWEN_COMPILER_NAME "Unknown Compiler"
    #define NUWEN_COMPILER_VERSION "(unknown version)"
#endif

#define NUWEN_COMPILER \
    "This program was compiled on " __DATE__ " at " __TIME__ "\n" \
    "by " NUWEN_COMPILER_NAME " " NUWEN_COMPILER_VERSION " using libnuwen " NUWEN_VERSION "."

#endif // Idempotency
