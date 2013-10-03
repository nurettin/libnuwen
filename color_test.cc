// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "color.hh"

#include "external_begin.hh"
    #include <cstdio>
    #include <cstdlib>
    #include <iostream>
    #include <ostream>
#include "external_end.hh"

using namespace std;
using namespace nuwen;

int main() {
    atexit(reset_color);

    set_fore_color(1, 1, 0, 0);
    set_back_color(0, 0, 0, 1);

    printf("printf(): Red on Dark Blue.\n");

    reset_color();

    cout << endl;

    cout << black      << "Black."       << endl;
    cout << darkblue   << "Dark Blue."   << endl;
    cout << darkgreen  << "Dark Green."  << endl;
    cout << darkcyan   << "Dark Cyan."   << endl;
    cout << darkred    << "Dark Red."    << endl;
    cout << darkpurple << "Dark Purple." << endl;
    cout << darkyellow << "Dark Yellow." << endl;
    cout << gray       << "Gray."        << endl;
    cout << darkgray   << "Dark Gray."   << endl;
    cout << blue       << "Blue."        << endl;
    cout << green      << "Green."       << endl;
    cout << cyan       << "Cyan."        << endl;
    cout << red        << "Red."         << endl;
    cout << purple     << "Purple."      << endl;
    cout << yellow     << "Yellow."      << endl;
    cout << white      << "White."       << endl;

    cout << black << endl;

    cout << bg_black      << "Black on Black."       << endl;
    cout << bg_darkblue   << "Black on Dark Blue."   << endl;
    cout << bg_darkgreen  << "Black on Dark Green."  << endl;
    cout << bg_darkcyan   << "Black on Dark Cyan."   << endl;
    cout << bg_darkred    << "Black on Dark Red."    << endl;
    cout << bg_darkpurple << "Black on Dark Purple." << endl;
    cout << bg_darkyellow << "Black on Dark Yellow." << endl;
    cout << bg_gray       << "Black on Gray."        << endl;
    cout << bg_darkgray   << "Black on Dark Gray."   << endl;
    cout << bg_blue       << "Black on Blue."        << endl;
    cout << bg_green      << "Black on Green."       << endl;
    cout << bg_cyan       << "Black on Cyan."        << endl;
    cout << bg_red        << "Black on Red."         << endl;
    cout << bg_purple     << "Black on Purple."      << endl;
    cout << bg_yellow     << "Black on Yellow."      << endl;
    cout << bg_white      << "Black on White."       << endl;

    cout << endl;

    cout << red        << bg_gray       << "Red on Gray."              << endl;
    cout << black      << bg_cyan       << "Black on Cyan."            << endl;
    cout << darkblue   << bg_green      << "Dark Blue on Green."       << endl;
    cout << darkcyan   << bg_yellow     << "Dark Cyan on Yellow."      << endl;
    cout << darkred    << bg_white      << "Dark Red on White."        << endl;
    cout << darkgreen  << bg_purple     << "Dark Green on Purple."     << endl;
    cout << darkgray   << bg_red        << "Dark Gray on Red."         << endl;
    cout << cyan       << bg_darkpurple << "Cyan on Dark Purple."      << endl;
    cout << darkpurple << bg_blue       << "Dark Purple on Blue."      << endl;
    cout << darkyellow << bg_darkgray   << "Dark Yellow on Dark Gray." << endl;
    cout << blue       << bg_darkyellow << "Blue on Dark Yellow."      << endl;
    cout << gray       << bg_black      << "Gray on Black."            << endl;
    cout << green      << bg_darkblue   << "Green on Dark Blue."       << endl;
    cout << purple     << bg_darkgreen  << "Purple on Dark Green."     << endl;
    cout << yellow     << bg_darkcyan   << "Yellow on Dark Cyan."      << endl;
    cout << white      << bg_darkred    << "White on Dark Red."        << endl;
    cout << green      << bg_green      << "Green on Green."           << endl;
}
