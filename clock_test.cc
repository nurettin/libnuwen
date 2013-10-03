// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "clock.hh"
#include "string.hh"
#include "test.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <algorithm>
    #include <cmath>
    #include <iostream>
    #include <iterator>
    #include <numeric>
    #include <ostream>
    #include <stdexcept>
#include "external_end.hh"

using namespace std;
using namespace nuwen;
using namespace nuwen::chrono;

bool sorted(const vull_t& v) {
    vull_t dupe(v);

    sort(dupe.begin(), dupe.end());

    return v == dupe;
}

double average(const dull_t& d) {
    if (d.empty()) {
        throw std::logic_error("LOGIC ERROR: average() - Insufficiently many elements.");
    }

    return accumulate(d.begin(), d.end(), 0.0) / d.size();
}

double std_dev(const dull_t& d) {
    if (d.size() < 2) {
        throw std::logic_error("LOGIC ERROR: std_dev() - Insufficiently many elements.");
    }

    double x = 0;

    const double avg = average(d);

    for (dull_ci_t i = d.begin(); i != d.end(); ++i) {
        x += pow(static_cast<double>(*i) - avg, 2);
    }

    return sqrt(x / (d.size() - 1));
}

int main() {
    vull_t samples(10000000, 0);

    generate(samples.begin(), samples.end(), clock_ctr);

    NUWEN_TEST("clock1", sorted(samples))

    dull_t deltas;

    adjacent_difference(samples.begin(), samples.end(), back_inserter(deltas));

    deltas.pop_front();

    cout << "  Samples collected: " << comma_from_ull(samples.size()) << endl;
    cout << "  Duplicate samples: " << count(deltas.begin(), deltas.end(), static_cast<ull_t>(0)) << endl;

    const double us_per_tick = 1000000.0 / static_cast<double>(clock_freq());

    cout << "Reported resolution: " << us_per_tick << " us" << endl;
    cout << "      Average delta: " << average(deltas) * us_per_tick << " us" << endl;
    cout << " Standard deviation: " << std_dev(deltas) * us_per_tick << " us" << endl;
    cout << "      Largest delta: " << static_cast<double>(*max_element(deltas.begin(), deltas.end())) * us_per_tick << " us" << endl;


    watch w;

    cout << endl << "Watch is running." << endl;

    cout << endl << "Snoozing for 2.5 seconds." << endl;
    snooze(2.5);
    cout << "Elapsed time: " << w.seconds() << " s" << endl;

    cout << endl << "Resetting watch." << endl;
    w.reset();

    cout << endl << "Snoozing for 3.1 seconds." << endl;
    snooze(3.1);
    cout << "Elapsed time: " << w.seconds() << " s" << endl;

    cout << endl << "Not resetting watch." << endl;

    cout << endl << "Snoozing for 1.7 seconds." << endl;
    snooze(1.7);
    cout << "Elapsed time: " << w.seconds() << " s" << endl;

    cout << endl;
}
