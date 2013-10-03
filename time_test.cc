// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "test.hh"
#include "time.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <iostream>
    #include <ostream>
    #include <string>
    #include <utility>
    #include <boost/date_time/gregorian/gregorian.hpp>
    #include <boost/date_time/posix_time/posix_time.hpp>
#include "external_end.hh"

using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace nuwen;

int main() {
    // Boost.Date_Time can't express UTC times that occur during leap seconds.
    // Theoretically speaking, TAI - UTC at 23:59:60 is identical to TAI - UTC at 23:59:59.

    NUWEN_TEST("time1",  tai_minus_utc_at_utc(ptime(date(2006, Jan,  1), time_duration( 0,  0,  1))) == 33)
    NUWEN_TEST("time2",  tai_minus_utc_at_utc(ptime(date(2006, Jan,  1), time_duration( 0,  0,  0))) == 33)
    NUWEN_TEST("time3",  tai_minus_utc_at_utc(ptime(date(2005, Dec, 31), time_duration(23, 59, 59))) == 32)

    NUWEN_TEST("time4",  tai_minus_utc_at_utc(ptime(date(1999, Jan,  1), time_duration( 0,  0,  1))) == 32)
    NUWEN_TEST("time5",  tai_minus_utc_at_utc(ptime(date(1999, Jan,  1), time_duration( 0,  0,  0))) == 32)
    NUWEN_TEST("time6",  tai_minus_utc_at_utc(ptime(date(1998, Dec, 31), time_duration(23, 59, 59))) == 31)

    NUWEN_TEST("time7",  tai_minus_utc_at_utc(ptime(date(1980, Jan,  1), time_duration( 0,  0,  1))) == 19)
    NUWEN_TEST("time8",  tai_minus_utc_at_utc(ptime(date(1980, Jan,  1), time_duration( 0,  0,  0))) == 19)
    NUWEN_TEST("time9",  tai_minus_utc_at_utc(ptime(date(1979, Dec, 31), time_duration(23, 59, 59))) == 18)

    NUWEN_TEST("time10", tai_minus_utc_at_utc(ptime(date(1950, Jan,  1), time_duration( 0,  0,  0))) ==  5)

    {
        const ptime a(date(2006, Jan,  1), time_duration( 0,  0,  1));
        const ptime b(date(2006, Jan,  1), time_duration( 0,  0,  0));
        const ptime c(date(2005, Dec, 31), time_duration(23, 59, 59));
        const ptime d(date(2005, Dec, 31), time_duration(23, 59, 58));

        NUWEN_TEST("time11", utc_minus(a, b) == 1)
        NUWEN_TEST("time12", utc_minus(b, c) == 2)
        NUWEN_TEST("time13", utc_minus(c, d) == 1)
        NUWEN_TEST("time14", utc_minus(a, d) == 4)

        NUWEN_TEST("time15", utc_minus(b, a) == -1)
        NUWEN_TEST("time16", utc_minus(c, b) == -2)
        NUWEN_TEST("time17", utc_minus(d, c) == -1)
        NUWEN_TEST("time18", utc_minus(d, a) == -4)
    }

    {
        const ptime arbitrary(date(2003, Jan, 2), time_duration(4, 5, 6));

        NUWEN_TEST("time19", utc_plus(arbitrary, 0) == make_pair(arbitrary, false))

        const ptime t2006(date(2006, Feb, 4), time_duration(8, 16, 32));
        const ptime t2007(date(2007, Feb, 4), time_duration(8, 16, 32));

        NUWEN_TEST("time20", utc_plus(t2006, 31536000) == make_pair(t2007, false))

        const ptime a(date(1989, Dec, 31), time_duration(23, 59, 58));
        const ptime b(date(1989, Dec, 31), time_duration(23, 59, 59));
        const ptime c(date(1990, Jan,  1), time_duration( 0,  0,  0));
        const ptime d(date(1990, Jan,  1), time_duration( 0,  0,  1));

        NUWEN_TEST("time21", utc_plus(a, 1) == make_pair(b, false))
        NUWEN_TEST("time22", utc_plus(a, 2) == make_pair(b, true))
        NUWEN_TEST("time23", utc_plus(a, 3) == make_pair(c, false))
        NUWEN_TEST("time24", utc_plus(a, 4) == make_pair(d, false))

        const ptime w(date(1990, Dec, 31), time_duration(23, 59, 57));

        const ptime x(date(1990, Dec, 31), time_duration(23, 59, 59));
        const ptime y(date(1991, Jan,  1), time_duration( 0,  0,  0));
        const ptime z(date(1991, Jan,  1), time_duration( 0,  0,  1));

        NUWEN_TEST("time25", utc_plus(a, 31536000) == make_pair(w, false))
        NUWEN_TEST("time26", utc_plus(a, 31536002) == make_pair(x, false))
        NUWEN_TEST("time27", utc_plus(a, 31536003) == make_pair(x, true))
        NUWEN_TEST("time28", utc_plus(a, 31536004) == make_pair(y, false))
        NUWEN_TEST("time29", utc_plus(a, 31536005) == make_pair(z, false))

        const ptime t1955(date(1955, Feb, 4), time_duration(8, 16, 32));
        const ptime t1954(date(1954, Feb, 4), time_duration(8, 16, 32));

        NUWEN_TEST("time30", utc_plus(t1955, -31536000) == make_pair(t1954, false))

        NUWEN_TEST("time31", utc_plus(z, -1) == make_pair(y, false))
        NUWEN_TEST("time32", utc_plus(z, -2) == make_pair(x, true))
        NUWEN_TEST("time33", utc_plus(z, -3) == make_pair(x, false))
        NUWEN_TEST("time34", utc_plus(z, -5) == make_pair(w, false))

        NUWEN_TEST("time35", utc_plus(z, -31536002) == make_pair(c, false))
        NUWEN_TEST("time36", utc_plus(z, -31536003) == make_pair(b, true))
        NUWEN_TEST("time37", utc_plus(z, -31536004) == make_pair(b, false))
        NUWEN_TEST("time38", utc_plus(z, -31536005) == make_pair(a, false))
    }

    {
        // We can't exactly test current_qh_time().
        // However, we can calculate by hand what the current_qh_time()
        // should have been for some specific universal_time() in the past.

        cout << "current_qh_time(): " << current_qh_time() << endl;

        const ptime utc_past(date(2006, Aug, 5), time_duration(17, 8, 2));

        const sll_t days =
               37  // whole years
            * 365  // days per year
            +   9  // leap days
            +  10  // days between Jul 21 and Jul 31
            +   1  // days between Jul 31 and Aug  1
            +   4; // days between Aug  1 and Aug  5

        const sll_t seconds = (17 * 3600 + 8 * 60 + 2) - (2 * 3600 + 56 * 60 + 15);

        const sll_t leap_seconds = 33 - 7;

        const sll_t qh_past = days * 86400 + seconds + leap_seconds;

        NUWEN_TEST("time39", qh_from_utc(utc_past) == qh_past)

        NUWEN_TEST("time40", utc_from_qh(qh_past) == make_pair(utc_past, false))
    }

    {
        const sll_t qh = qh_from_utc(ptime(date(2006, Feb, 3), time_duration(4, 5, 6)));

        // Test my favorite format.
        NUWEN_TEST("time41", format_qh(qh, 0, false, "[M/D/YYYY DDD h:mm.ss AA]") == "[2/3/2006 Fri 4:05.06 AM]")

        // Test another common format.
        NUWEN_TEST("time42", format_qh(qh, 0, false, "[YYYY.MM.DD HH:mm:ss]") == "[2006.02.03 04:05:06]")

        // Test the rest of the Qeng Ho Time Format with ugly formats.
        NUWEN_TEST("time43", format_qh(qh, 0, false, "<MMMM (MMM) Do {***THAT'S THE*** DO **!**}, YY [*a* DDDD]>")
            == "<February (Feb) 3rd {*THAT'S THE* 3RD *!*}, 06 [a Friday]>")

        NUWEN_TEST("time44", format_qh(qh, 0, false, "aAaaHrRhhms") == "aAam4mmviMMVI0456")

        NUWEN_TEST("time45", format_qh(1047000002, -8, false, "q|qq") == "1047000002|1,047,000,002")

        // Attempt to use reserved sequences.
        NUWEN_TEST("time46", format_qh(qh, 0, false, "xxQYhhh")
            == "[Unrecognized: xx][Unrecognized: Q][Unrecognized: Y][Unrecognized: hhh]")

        // Test a leap second.
        NUWEN_TEST("time47", format_qh(qh_from_utc(ptime(date(1998, Dec, 31), time_duration(23, 59, 59))) + 1, 0, false,
            "[M/D/YYYY DDD h:mm.ss AA]") == "[12/31/1998 Thu 11:59.60 PM]")

        // Test DST.
        const string s("[M/D/YYYY DDD h:mm.ss AA *P*TT (*p*tt)]");

        NUWEN_TEST("time48", format_qh(qh_from_utc(ptime(date(2006, Apr, 2), time_duration(9, 59, 59))), -8, true, s)
            == "[4/2/2006 Sun 1:59.59 AM PST (pst)]")
        NUWEN_TEST("time49", format_qh(qh_from_utc(ptime(date(2006, Apr, 2), time_duration(10, 0, 0))), -8, true, s)
            == "[4/2/2006 Sun 3:00.00 AM PDT (pdt)]")
        NUWEN_TEST("time50", format_qh(qh_from_utc(ptime(date(2006, Apr, 2), time_duration(10, 0, 1))), -8, true, s)
            == "[4/2/2006 Sun 3:00.01 AM PDT (pdt)]")

        NUWEN_TEST("time51", format_qh(qh_from_utc(ptime(date(2006, Oct, 29), time_duration(8, 59, 59))), -8, true, s)
            == "[10/29/2006 Sun 1:59.59 AM PDT (pdt)]")
        NUWEN_TEST("time52", format_qh(qh_from_utc(ptime(date(2006, Oct, 29), time_duration(9, 0, 0))), -8, true, s)
            == "[10/29/2006 Sun 1:00.00 AM PST (pst)]")
        NUWEN_TEST("time53", format_qh(qh_from_utc(ptime(date(2006, Oct, 29), time_duration(9, 0, 1))), -8, true, s)
            == "[10/29/2006 Sun 1:00.01 AM PST (pst)]")

        NUWEN_TEST("time54", format_qh(qh_from_utc(ptime(date(2007, Mar, 11), time_duration(9, 59, 59))), -8, true, s)
            == "[3/11/2007 Sun 1:59.59 AM PST (pst)]")
        NUWEN_TEST("time55", format_qh(qh_from_utc(ptime(date(2007, Mar, 11), time_duration(10, 0, 0))), -8, true, s)
            == "[3/11/2007 Sun 3:00.00 AM PDT (pdt)]")
        NUWEN_TEST("time56", format_qh(qh_from_utc(ptime(date(2007, Mar, 11), time_duration(10, 0, 1))), -8, true, s)
            == "[3/11/2007 Sun 3:00.01 AM PDT (pdt)]")

        NUWEN_TEST("time57", format_qh(qh_from_utc(ptime(date(2007, Nov, 4), time_duration(8, 59, 59))), -8, true, s)
            == "[11/4/2007 Sun 1:59.59 AM PDT (pdt)]")
        NUWEN_TEST("time58", format_qh(qh_from_utc(ptime(date(2007, Nov, 4), time_duration(9, 0, 0))), -8, true, s)
            == "[11/4/2007 Sun 1:00.00 AM PST (pst)]")
        NUWEN_TEST("time59", format_qh(qh_from_utc(ptime(date(2007, Nov, 4), time_duration(9, 0, 1))), -8, true, s)
            == "[11/4/2007 Sun 1:00.01 AM PST (pst)]")

        // Test AM/PM, Hour-12, and Hour-24.
        NUWEN_TEST("time60", format_qh(qh_from_utc(ptime(date(2006, Feb, 3), hours(0))), 0, false, "h hh H HH AA") == "12 12 0 00 AM")
        NUWEN_TEST("time61", format_qh(qh_from_utc(ptime(date(2006, Feb, 3), hours(5))), 0, false, "h hh H HH AA") == "5 05 5 05 AM")
        NUWEN_TEST("time62", format_qh(qh_from_utc(ptime(date(2006, Feb, 3), hours(11))), 0, false, "h hh H HH AA") == "11 11 11 11 AM")
        NUWEN_TEST("time63", format_qh(qh_from_utc(ptime(date(2006, Feb, 3), hours(12))), 0, false, "h hh H HH AA") == "12 12 12 12 PM")
        NUWEN_TEST("time64", format_qh(qh_from_utc(ptime(date(2006, Feb, 3), hours(13))), 0, false, "h hh H HH AA") == "1 01 13 13 PM")
        NUWEN_TEST("time65", format_qh(qh_from_utc(ptime(date(2006, Feb, 3), hours(20))), 0, false, "h hh H HH AA") == "8 08 20 20 PM")
        NUWEN_TEST("time66", format_qh(qh_from_utc(ptime(date(2006, Feb, 3), hours(23))), 0, false, "h hh H HH AA") == "11 11 23 23 PM")

        // Test a negative time.
        NUWEN_TEST("time67", format_qh(-137, 0, false, "[M/D/YYYY DDD h:mm.ss AA]") == "[7/21/1969 Mon 2:53.58 AM]")
    }
}
