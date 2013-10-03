// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_COLOR_HH
#define PHAM_COLOR_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "color_private.hh"

#include "external_begin.hh"
    #include <ostream>

    #ifdef NUWEN_PLATFORM_UNIX
        #include <cstdio>
    #endif
#include "external_end.hh"

#define PHAM_COLOR_FXN(PHAM_NAME, PHAM_FOREBACK, PHAM_BOLD, PHAM_RED, PHAM_GREEN, PHAM_BLUE) \
    template <typename C, typename T> std::basic_ostream<C, T>& \
        PHAM_NAME(std::basic_ostream<C, T>& os) { \
            set_ ## PHAM_FOREBACK ## _color(PHAM_BOLD, PHAM_RED, PHAM_GREEN, PHAM_BLUE); return os; \
        }

#define PHAM_COLOR_FXNS(PHAM_NAME, PHAM_BOLD, PHAM_RED, PHAM_GREEN, PHAM_BLUE) \
    PHAM_COLOR_FXN(PHAM_NAME, fore, PHAM_BOLD, PHAM_RED, PHAM_GREEN, PHAM_BLUE) \
    PHAM_COLOR_FXN(bg_ ## PHAM_NAME, back, PHAM_BOLD, PHAM_RED, PHAM_GREEN, PHAM_BLUE)

namespace nuwen {
    inline void set_fore_color(bool bold, bool red, bool green, bool blue);
    inline void set_back_color(bool bold, bool red, bool green, bool blue);
    inline void reset_color();

    PHAM_COLOR_FXNS(black,      0, 0, 0, 0)
    PHAM_COLOR_FXNS(darkblue,   0, 0, 0, 1)
    PHAM_COLOR_FXNS(darkgreen,  0, 0, 1, 0)
    PHAM_COLOR_FXNS(darkcyan,   0, 0, 1, 1)
    PHAM_COLOR_FXNS(darkred,    0, 1, 0, 0)
    PHAM_COLOR_FXNS(darkpurple, 0, 1, 0, 1)
    PHAM_COLOR_FXNS(darkyellow, 0, 1, 1, 0)
    PHAM_COLOR_FXNS(gray,       0, 1, 1, 1)
    PHAM_COLOR_FXNS(darkgray,   1, 0, 0, 0)
    PHAM_COLOR_FXNS(blue,       1, 0, 0, 1)
    PHAM_COLOR_FXNS(green,      1, 0, 1, 0)
    PHAM_COLOR_FXNS(cyan,       1, 0, 1, 1)
    PHAM_COLOR_FXNS(red,        1, 1, 0, 0)
    PHAM_COLOR_FXNS(purple,     1, 1, 0, 1)
    PHAM_COLOR_FXNS(yellow,     1, 1, 1, 0)
    PHAM_COLOR_FXNS(white,      1, 1, 1, 1)
}

#undef PHAM_COLOR_FXN
#undef PHAM_COLOR_FXNS

#ifdef NUWEN_PLATFORM_WINDOWS
    namespace pham {
        inline void set_win32_color(const int color, const int mask) {
            CONSOLE_SCREEN_BUFFER_INFO s;
            const HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE_CONSTANT);

            if (h != INVALID_HANDLE_VALUE_CONSTANT && h != NULL && GetConsoleScreenBufferInfo(h, &s) != 0) {
                SetConsoleTextAttribute(h, static_cast<WORD>(color | (s.wAttributes & mask)));
            }
        }
    }

    inline void nuwen::set_fore_color(const bool bold, const bool red, const bool green, const bool blue) {
        pham::set_win32_color(
            bold * FOREGROUND_INTENSITY | red * FOREGROUND_RED | green * FOREGROUND_GREEN | blue * FOREGROUND_BLUE,
            BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE
        );
    }

    inline void nuwen::set_back_color(const bool bold, const bool red, const bool green, const bool blue) {
        pham::set_win32_color(
            bold * BACKGROUND_INTENSITY | red * BACKGROUND_RED | green * BACKGROUND_GREEN | blue * BACKGROUND_BLUE,
            FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
        );
    }
#endif

#ifdef NUWEN_PLATFORM_UNIX
    inline void nuwen::set_fore_color(const bool bold, const bool red, const bool green, const bool blue) {
        std::printf("\033[3%c;%sm", '0' + blue * 4 + green * 2 + red, bold ? "1" : "22");
    }

    inline void nuwen::set_back_color(const bool bold, const bool red, const bool green, const bool blue) {
        std::printf("\033[4%c;%sm", '0' + blue * 4 + green * 2 + red, bold ? "5" : "25");
    }
#endif

inline void nuwen::reset_color() {
    set_fore_color(0, 1, 1, 1);
    set_back_color(0, 0, 0, 0);
}

#endif // Idempotency
