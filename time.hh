// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_TIME_HH
#define PHAM_TIME_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "gluon.hh"
#include "string.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <algorithm>
    #include <cctype>
    #include <stdexcept>
    #include <string>
    #include <utility>
    #include <vector>
    #include <boost/date_time/gregorian/gregorian.hpp>
    #include <boost/date_time/posix_time/posix_time.hpp>
    #include <boost/lexical_cast.hpp>
#include "external_end.hh"

namespace nuwen {
    // libnuwen uses Qeng Ho (QH) time, which is defined as the number of
    // seconds since Neil Armstrong first stepped onto the lunar surface.

    // Why use yet another epoch? Its main competitors are UTC and TAI. The
    // wretched monstrosity that is UTC isn't even continuous, being
    // repeatedly polluted with leap seconds. Worse, UTC between 1961 and
    // 1972 didn't even run at one UTC second per SI second. Worst of all,
    // UTC wasn't defined before 1961. TAI is somewhat better, being
    // continuous. However, it is connected to the vile Common Era, and we
    // can't have that.

    // Unix time is beyond evil.

    // QH time has the following advantages:
    // 1. It is continuous, making arithmetic easy.
    // 2. It is defined arbitrarily far into the past and into the future.
    // 3. It is always stored in a sll_t, avoiding vile wrapping problems.
    // 4. It is readily explainable to anyone anywhere, as it is not
    // connected to the vile Common Era.

    // Of course, no one else speaks QH time yet, so libnuwen must go to
    // some length in order to obtain the current QH time, convert QH to
    // UTC and back, and format QH time into something locally
    // comprehensible.

    inline sl_t tai_minus_utc_at_utc(const boost::posix_time::ptime& utc);

    inline sll_t utc_minus(const boost::posix_time::ptime& utc_l, const boost::posix_time::ptime& utc_r);

    inline std::pair<boost::posix_time::ptime, bool> utc_plus(boost::posix_time::ptime utc, sll_t n);

    inline sll_t qh_from_utc(const boost::posix_time::ptime& utc);

    inline std::pair<boost::posix_time::ptime, bool> utc_from_qh(sll_t qh);

    inline sll_t current_qh_time();

    // Qeng Ho Time Format

    // YY / YYYY   (Y)ear           (evil) / (good)    06 / 2006
    // tt / TT     DS(T)           (lower) / (upper)   st/dt / ST/DT
    // M / MM      (M)onth          (bare) / (padded)  1-12 / 01-12
    // MMM / MMMM  (M)onth        (abbrev) / (full)    Jan / January
    // D / DD      (D)ay            (bare) / (padded)  1-31 / 01-31
    // o / O       Day (O)rdinal   (lower) / (upper)   th / TH
    // DDD / DDDD  (D)ay          (abbrev) / (full)    Mon / Monday
    // a / A       (A)M/PM         (lower) / (upper)   a/p / A/P
    // aa / AA     (A)M/PM         (lower) / (upper)   am/pm / AM/PM
    // H / HH      (H)our-24        (bare) / (padded)  0-23 / 00-23
    // h / hh      (H)our-12        (bare) / (padded)  1-12 / 01-12
    // m / mm      (M)inute         (bare) / (padded)  0-59 / 00-59
    // s / ss      (S)econd         (bare) / (padded)  0-60 / 00-60
    // q / qq      (Q)eng Ho       (solid) / (comma)   1035424059 / 1,035,424,059
    // r / R       (R)oman Year    (lower) / (upper)   mmvi / MMVI

    // All alphabetic characters are reserved, while all other characters are literal.
    // Surround a sequence of characters with stars *like this* to make it literal.
    // An empty pair of stars will be replaced with a single star.

    inline std::string format_qh(sll_t qh, int hours_offset, bool use_dst, const std::string& fmt);
}

namespace pham {
    namespace leap {
        using namespace std;
        using namespace boost;
        using namespace boost::gregorian;
        using namespace boost::posix_time;
        using namespace nuwen;

        // The QH epoch, expressed in UTC. Unfortunately, UTC is complicated,
        // so using this constant requires careful attention.
        const ptime utc_armstrong(date(1969, Jul, 21), time_duration(2, 56, 15));

        typedef pair<sl_t, ptime> entry_t;
        typedef vector<entry_t> codex_t;
        typedef codex_t::const_iterator codex_ci_t;

        const codex_t codex = vec(glu<entry_t>

            // The entry (N, T) means that on and after time T, TAI - UTC == N.

            (entry_t(34, ptime(date(2009, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(33, ptime(date(2006, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(32, ptime(date(1999, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(31, ptime(date(1997, Jul, 1), time_duration(0, 0, 0))))
            (entry_t(30, ptime(date(1996, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(29, ptime(date(1994, Jul, 1), time_duration(0, 0, 0))))
            (entry_t(28, ptime(date(1993, Jul, 1), time_duration(0, 0, 0))))
            (entry_t(27, ptime(date(1992, Jul, 1), time_duration(0, 0, 0))))
            (entry_t(26, ptime(date(1991, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(25, ptime(date(1990, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(24, ptime(date(1988, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(23, ptime(date(1985, Jul, 1), time_duration(0, 0, 0))))
            (entry_t(22, ptime(date(1983, Jul, 1), time_duration(0, 0, 0))))
            (entry_t(21, ptime(date(1982, Jul, 1), time_duration(0, 0, 0))))
            (entry_t(20, ptime(date(1981, Jul, 1), time_duration(0, 0, 0))))
            (entry_t(19, ptime(date(1980, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(18, ptime(date(1979, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(17, ptime(date(1978, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(16, ptime(date(1977, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(15, ptime(date(1976, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(14, ptime(date(1975, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(13, ptime(date(1974, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(12, ptime(date(1973, Jan, 1), time_duration(0, 0, 0))))
            (entry_t(11, ptime(date(1972, Jul, 1), time_duration(0, 0, 0))))
            (entry_t(10, ptime(date(1972, Jan, 1), time_duration(0, 0, 0))))

            // The following are fake leap seconds that accurately model the relationship
            // between TAI and UTC.

            (entry_t( 9, ptime(date(1971, Jan, 1), time_duration(0, 0, 0))))
            (entry_t( 8, ptime(date(1970, Jan, 1), time_duration(0, 0, 0))))
            (entry_t( 7, ptime(date(1969, Jan, 1), time_duration(0, 0, 0))))
            (entry_t( 6, ptime(date(1968, Jan, 1), time_duration(0, 0, 0))))

            // UTC actually goes back to 1961. However, I don't care enough to compute
            // when TAI - UTC == 5 actually occurred.

            (entry_t( 5, ptime(neg_infin))));

        inline bool entry_greater(const entry_t& l, const entry_t& r);

        inline codex_ci_t codex_lookup(const ptime& utc);

        inline ptime dst_correction(const ptime& local_std);

        inline string replace_chars(char c, string::size_type n,
            const ptime& local, bool leap_second, sll_t qh, bool dst_in_effect);

        inline string pad_to_two_digits(const int i) {
            return (i < 10 ? "0" : "") + lexical_cast<string>(i);
        }

        inline string daylite(const bool dst) {
            return dst ? "dt" : "st";
        }

        inline string str_from_month(const int m) {
            switch (m) {
                case 1: return "January";
                case 2: return "February";
                case 3: return "March";
                case 4: return "April";
                case 5: return "May";
                case 6: return "June";
                case 7: return "July";
                case 8: return "August";
                case 9: return "September";
                case 10: return "October";
                case 11: return "November";
                case 12: return "December";
                default: throw logic_error("LOGIC ERROR: pham::leap::str_from_month() - Invalid m.");
            }
        }

        inline string ordinal(const int i) {
            switch (i % 10) {
                case 1: return "st";
                case 2: return "nd";
                case 3: return "rd";
                default: return "th";
            }
        }

        inline string str_from_weekday(const int d) {
            switch (d) {
                case 0: return "Sunday";
                case 1: return "Monday";
                case 2: return "Tuesday";
                case 3: return "Wednesday";
                case 4: return "Thursday";
                case 5: return "Friday";
                case 6: return "Saturday";
                default: throw logic_error("LOGIC ERROR: pham::leap::str_from_weekday() - Invalid d.");
            }
        }

        inline string meridiem_sm(const int t) {
            return t < 12 ? "a" : "p";
        }

        inline string meridiem_lg(const int t) {
            return t < 12 ? "am" : "pm";
        }

        inline int hour_12_from_24(const int t) {
            if (t < 0 || t > 23) {
                throw logic_error("LOGIC ERROR: pham::leap::hour_12_from_24() - Invalid t.");
            } else if (t == 0) {
                return 12;
            } else if (t < 13) {
                return t;
            } else {
                return t - 12;
            }
        }

        inline string roman_numeral(const us_t n) {
            if (n == 0) {
                throw logic_error("LOGIC ERROR: pham::leap::roman_numeral() - Invalid n.");
            }

            static const vs_t hundreds = vec(glu<string>("")("C")("CC")("CCC")("CD")("D")("DC")("DCC")("DCCC")("CM"));
            static const vs_t     tens = vec(glu<string>("")("X")("XX")("XXX")("XL")("L")("LX")("LXX")("LXXX")("XC"));
            static const vs_t     ones = vec(glu<string>("")("I")("II")("III")("IV")("V")("VI")("VII")("VIII")("IX"));

            return string(n / 1000, 'M') + hundreds[n / 100 % 10] + tens[n / 10 % 10] + ones[n % 10];
        }
    }
}

inline nuwen::sl_t nuwen::tai_minus_utc_at_utc(const boost::posix_time::ptime& utc) {
    return pham::leap::codex_lookup(utc)->first;
}

inline nuwen::sll_t nuwen::utc_minus(const boost::posix_time::ptime& utc_l, const boost::posix_time::ptime& utc_r) {
    const sll_t uncorrected_difference = (utc_l - utc_r).total_seconds();

    const sl_t offset_l = tai_minus_utc_at_utc(utc_l);
    const sl_t offset_r = tai_minus_utc_at_utc(utc_r);

    const sl_t correction = offset_l - offset_r;

    return uncorrected_difference + correction;
}

inline std::pair<boost::posix_time::ptime, bool> nuwen::utc_plus(boost::posix_time::ptime utc, sll_t n) {
    using namespace pham::leap;

    codex_ci_t i = codex_lookup(utc);

    while (n != 0) {
        if (n > 0) {
            if (i == codex.begin()) {
                // utc occurs after the last known leap second.
                // Therefore, adding to utc can't encounter any more leap seconds.
                return make_pair(utc + seconds(static_cast<sl_t>(n)), false);
            }

            --i;

            const sll_t leap_eta = (i->second - utc).total_seconds();

            if (n < leap_eta) {
                // We don't encounter a leap second.
                return make_pair(utc + seconds(static_cast<sl_t>(n)), false);
            } else if (n == leap_eta) {
                // We exactly hit a leap second.
                return make_pair(i->second - seconds(1), true);
            } else {
                // We step over a leap second.
                utc = i->second;
                n -= leap_eta + 1;
            }
        } else {
            if (i->second.is_neg_infinity()) {
                // utc occurs before the first known leap second.
                // Therefore, subtracting from utc can't encounter any more leap seconds.
                return make_pair(utc + seconds(static_cast<sl_t>(n)), false);
            }

            const sll_t mag = -n;

            const sll_t leap_eta = (utc - i->second).total_seconds() + 1;

            if (mag < leap_eta) {
                // We don't encounter a leap second.
                return make_pair(utc + seconds(static_cast<sl_t>(n)), false);
            } else if (mag == leap_eta) {
                // We exactly hit a leap second.
                return make_pair(i->second - seconds(1), true);
            } else {
                // We step over a leap second.
                utc = i->second - seconds(1);
                n += leap_eta + 1;
                ++i;
            }
        }
    }

    return make_pair(utc, false);
}

inline nuwen::sll_t nuwen::qh_from_utc(const boost::posix_time::ptime& utc) {
    return utc_minus(utc, pham::leap::utc_armstrong);
}

inline std::pair<boost::posix_time::ptime, bool> nuwen::utc_from_qh(const sll_t qh) {
    return utc_plus(pham::leap::utc_armstrong, qh);
}

inline nuwen::sll_t nuwen::current_qh_time() {
    return qh_from_utc(boost::posix_time::second_clock::universal_time());
}

inline std::string nuwen::format_qh(const sll_t qh, const int hours_offset, const bool use_dst, const std::string& fmt) {
    using namespace pham::leap;

    const pair<ptime, bool> p = utc_from_qh(qh);

    const bool leap_second = p.second;

    const ptime local_std = p.first + hours(hours_offset);

    const ptime local = use_dst ? dst_correction(local_std) : local_std;

    const bool dst_in_effect = local != local_std;

    string ret;
    bool in_quote = false;
    bool quoted_any = false;

    for (string::const_iterator i = fmt.begin(); i != fmt.end(); ++i) {
        const char c = *i;

        if (c == '*') {
            if (in_quote && !quoted_any) {
                ret += c;
            }

            in_quote = !in_quote;
            quoted_any = false;
        } else if (in_quote) {
            ret += c;
            quoted_any = true;
        } else if (!isalpha(c)) {
            ret += c;
        } else {
            string::size_type n = 1;

            while (i + 1 != fmt.end() && *(i + 1) == c) {
                ++i;
                ++n;
            }

            ret += replace_chars(c, n, local, leap_second, qh, dst_in_effect);
        }
    }

    return ret;
}

inline bool pham::leap::entry_greater(const entry_t& l, const entry_t& r) {
    return l.second > r.second;
}

inline pham::leap::codex_ci_t pham::leap::codex_lookup(const boost::posix_time::ptime& utc) {
    const codex_ci_t i = lower_bound(codex.begin(), codex.end(), entry_t(0, utc), entry_greater);

    if (i == codex.end()) {
        throw logic_error("LOGIC ERROR: pham::leap::codex_lookup() - Bogus iterator.");
    }

    return i;
}

inline boost::posix_time::ptime pham::leap::dst_correction(const boost::posix_time::ptime& local_std) {
    const us_t year = local_std.date().year();

    ptime begin;
    ptime end;

    if (year < 2007) {
        begin = ptime(first_day_of_the_week_in_month(Sunday, Apr).get_date(year), hours(2));
          end = ptime( last_day_of_the_week_in_month(Sunday, Oct).get_date(year), hours(2));
    } else {
        typedef nth_day_of_the_week_in_month nth_dow;

        begin = ptime(      nth_dow(nth_dow::second, Sunday, Mar).get_date(year), hours(2));
          end = ptime(first_day_of_the_week_in_month(Sunday, Nov).get_date(year), hours(2));
    }

    if (local_std < begin) {
        return local_std; // DST is not yet in effect.
    }

    const ptime local_dst = local_std + hours(1);

    if (local_dst < end) {
        return local_dst; // DST is in effect.
    }

    return local_std; // DST is no longer in effect.
}

inline std::string pham::leap::replace_chars(
    const char c, const std::string::size_type n, const boost::posix_time::ptime& local,
    const bool leap_second, const nuwen::sll_t qh, const bool dst_in_effect) {

    const us_t year = local.date().year();
    const int month = local.date().month();
    const int day = local.date().day();
    const int dow = local.date().day_of_week();
    const int hours = local.time_of_day().hours();
    const int minutes = local.time_of_day().minutes();
    const int seconds = leap_second ? 60 : local.time_of_day().seconds();

    const string s(n, c);

    if (s == "YY"  ) { return    pad_to_two_digits(year % 100); }
    if (s == "YYYY") { return lexical_cast<string>(year); }
    if (s == "tt"  ) { return lower(daylite(dst_in_effect)); }
    if (s == "TT"  ) { return upper(daylite(dst_in_effect)); }
    if (s == "M"   ) { return lexical_cast<string>(month); }
    if (s == "MM"  ) { return    pad_to_two_digits(month); }
    if (s == "MMM" ) { return str_from_month(month).substr(0, 3); }
    if (s == "MMMM") { return str_from_month(month); }
    if (s == "D"   ) { return lexical_cast<string>(day); }
    if (s == "DD"  ) { return    pad_to_two_digits(day); }
    if (s == "o"   ) { return lower(ordinal(day)); }
    if (s == "O"   ) { return upper(ordinal(day)); }
    if (s == "DDD" ) { return str_from_weekday(dow).substr(0, 3); }
    if (s == "DDDD") { return str_from_weekday(dow); }
    if (s == "a"   ) { return lower(meridiem_sm(hours)); }
    if (s == "A"   ) { return upper(meridiem_sm(hours)); }
    if (s == "aa"  ) { return lower(meridiem_lg(hours)); }
    if (s == "AA"  ) { return upper(meridiem_lg(hours)); }
    if (s == "H"   ) { return lexical_cast<string>(hours); }
    if (s == "HH"  ) { return    pad_to_two_digits(hours); }
    if (s == "h"   ) { return lexical_cast<string>(hour_12_from_24(hours)); }
    if (s == "hh"  ) { return    pad_to_two_digits(hour_12_from_24(hours)); }
    if (s == "m"   ) { return lexical_cast<string>(minutes); }
    if (s == "mm"  ) { return    pad_to_two_digits(minutes); }
    if (s == "s"   ) { return lexical_cast<string>(seconds); }
    if (s == "ss"  ) { return    pad_to_two_digits(seconds); }
    if (s == "q"   ) { return lexical_cast<string>(qh); }
    if (s == "qq"  ) { return       comma_from_sll(qh); }
    if (s == "r"   ) { return lower(roman_numeral(year)); }
    if (s == "R"   ) { return upper(roman_numeral(year)); }

    return "[Unrecognized: " + s + "]";
}

#endif // Idempotency
