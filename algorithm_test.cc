// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "algorithm.hh"
#include "gluon.hh"
#include "string.hh"
#include "test.hh"
#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <algorithm>
    #include <functional>
    #include <iterator>
    #include <list>
    #include <string>
    #include <vector>
#include "external_end.hh"

using namespace std;
using namespace nuwen;

bool is_even(const uc_t x) {
    return x % 2 == 0;
}

bool is_odd(const uc_t x) {
    return x % 2 != 0;
}

bool less_size(const string& x, const string& y) {
    return x.size() < y.size();
}

bool case_insensitive_equal_to(const string& x, const string& y) {
    return lower(x) == lower(y);
}

namespace {
    class thing {
    public:
        thing() { }

        thing(const string& s) : m_s(s) { }

        bool operator<(const thing& rhs) const {
            return less_size(m_s, rhs.m_s);
        }

        bool operator==(const thing& rhs) const {
            return case_insensitive_equal_to(m_s, rhs.m_s);
        }

        bool operator==(const string& rhs) const {
            return m_s == rhs;
        }

        bool all_caps() const {
            return m_s == upper(m_s);
        }

        void make_lower() {
            m_s = lower(m_s);
        }

        string::size_type size() const {
            return m_s.size();
        }

    private:
        string m_s;
    };
}

int main() {
    NUWEN_TEST("thing1", thing("bbb") < thing("aaaa"))
    NUWEN_TEST("thing2", !(thing("bbb") < thing("aaa")))
    NUWEN_TEST("thing3", !(thing("bbb") < thing("aa")))

    NUWEN_TEST("thing4", thing("cat") == thing("cat"))
    NUWEN_TEST("thing5", thing("cat") == thing("CAT"))
    NUWEN_TEST("thing6", !(thing("cat") == thing("dog")))

    NUWEN_TEST("thing7", thing("cat") == string("cat"))
    NUWEN_TEST("thing8", !(thing("cat") == string("CAT")))
    NUWEN_TEST("thing9", !(thing("cat") == string("dog")))

    NUWEN_TEST("thing10", thing("cat") == "cat")
    NUWEN_TEST("thing11", !(thing("cat") == "CAT"))
    NUWEN_TEST("thing12", !(thing("cat") == "dog"))

    NUWEN_TEST("thing13", !thing("cat").all_caps())
    NUWEN_TEST("thing14", !thing("Cat").all_caps())
    NUWEN_TEST("thing15", thing("CAT").all_caps())

    {
        thing t("FOObarBAZ");
        t.make_lower();
        NUWEN_TEST("thing16", t == "foobarbaz")
    }

    NUWEN_TEST("thing17", thing("foobar").size() == 6)

    const vuc_t orig   = vec(glu<uc_t>(16)(1)(7)(137)(0)(2)(4)(16)(4)(1)(196)(33)(1)(7)(137)(0)(5)(4)(7)(47));
    const vuc_t even   = vec(glu<uc_t>(16)(0)(2)(4)(16)(4)(196)(0)(4));
    const vuc_t up     = vec(glu<uc_t>(0)(0)(1)(1)(1)(2)(4)(4)(4)(5)(7)(7)(7)(16)(16)(33)(47)(137)(137)(196));
    const vuc_t down   = vec(glu<uc_t>(196)(137)(137)(47)(33)(16)(16)(7)(7)(7)(5)(4)(4)(4)(2)(1)(1)(1)(0)(0));
    const vuc_t nofour = vec(glu<uc_t>(16)(1)(7)(137)(0)(2)(16)(1)(196)(33)(1)(7)(137)(0)(5)(7)(47));
    const vuc_t uniq   = vec(glu<uc_t>(0)(1)(2)(4)(5)(7)(16)(33)(47)(137)(196));

    {
        vuc_t v;
        copy_if(orig.begin(), orig.end(), back_inserter(v), is_even);
        NUWEN_TEST("algorithm1", v == even)
    }

    {
        vuc_t v = orig;
        universal_sort(v);
        NUWEN_TEST("algorithm2", v == up)
    }

    {
        luc_t l = sequence_cast<luc_t>(orig);
        universal_sort(l);
        NUWEN_TEST("algorithm3", l == sequence_cast<luc_t>(up))
    }

    {
        vuc_t v = orig;
        universal_sort(v, greater<uc_t>());
        NUWEN_TEST("algorithm4", v == down)
    }

    {
        luc_t l = sequence_cast<luc_t>(orig);
        universal_sort(l, greater<uc_t>());
        NUWEN_TEST("algorithm5", l == sequence_cast<luc_t>(down))
    }

    NUWEN_TEST("algorithm6", universal_sort_copy(orig) == up)
    NUWEN_TEST("algorithm7", universal_sort_copy(orig, greater<uc_t>()) == down)

    {
        vuc_t v = orig;
        nuke(v, 4);
        NUWEN_TEST("algorithm8", v == nofour)
    }

    NUWEN_TEST("algorithm9", nuke_copy(orig, 4) == nofour)

    {
        vuc_t v = orig;
        nuke_if(v, is_odd);
        NUWEN_TEST("algorithm10", v == even)
    }

    NUWEN_TEST("algorithm11", nuke_copy_if(orig, is_odd) == even)

    {
        vuc_t v = orig;
        nuke_dupes(v);
        NUWEN_TEST("algorithm12", v == uniq)
    }

    NUWEN_TEST("algorithm13", nuke_dupes_copy(orig) == uniq)


    const vs_t meow = vec(glu<string>("Kitty")("FLUFFY")("cute")("CUTE")("fluffy")("Cute")("KITTY")("Fluffy")("kitty"));
    const vs_t purr = vec(glu<string>("cute")("kitty")("fluffy"));
    const vs_t stab = vec(glu<string>("cute")("CUTE")("Cute")("Kitty")("KITTY")("kitty")("FLUFFY")("fluffy")("Fluffy"));
    const vs_t solo = vec(glu<string>("cute")("Kitty")("FLUFFY"));

    {
        vs_t v = meow;
        nuke_dupes(v, less_size, case_insensitive_equal_to);
        transform(v.begin(), v.end(), v.begin(), lower);
        NUWEN_TEST("algorithm14", v == purr)
    }

    {
        vs_t v = nuke_dupes_copy(meow, less_size, case_insensitive_equal_to);
        transform(v.begin(), v.end(), v.begin(), lower);
        NUWEN_TEST("algorithm15", v == purr)
    }

    {
        vs_t v = meow;
        universal_stable_sort(v, less_size);
        NUWEN_TEST("algorithm16", v == stab)
    }

    {
        ls_t l = sequence_cast<ls_t>(meow);
        universal_stable_sort(l, less_size);
        NUWEN_TEST("algorithm17", l == sequence_cast<ls_t>(stab))
    }

    NUWEN_TEST("algorithm18", universal_stable_sort_copy(meow, less_size) == stab)

    {
        vs_t v = meow;
        stable_nuke_dupes(v, less_size, case_insensitive_equal_to);
        NUWEN_TEST("algorithm19", v == solo)
    }

    NUWEN_TEST("algorithm20", stable_nuke_dupes_copy(meow, less_size, case_insensitive_equal_to) == solo)


    const vector<thing> meow_thing(meow.begin(), meow.end());

    {
        vector<thing> v = meow_thing;
        universal_stable_sort(v);
        NUWEN_TEST("algorithm21", v.size() == stab.size())
        NUWEN_TEST("algorithm22", equal(v.begin(), v.end(), stab.begin()))
    }

    {
        list<thing> l = sequence_cast<list<thing> >(meow_thing);
        universal_stable_sort(l);
        NUWEN_TEST("algorithm23", l.size() == stab.size())
        NUWEN_TEST("algorithm24", equal(l.begin(), l.end(), stab.begin()))
    }

    {
        const vector<thing> v = universal_stable_sort_copy(meow_thing);
        NUWEN_TEST("algorithm25", v.size() == stab.size())
        NUWEN_TEST("algorithm26", equal(v.begin(), v.end(), stab.begin()))
    }

    {
        vector<thing> v = meow_thing;
        stable_nuke_dupes(v);
        NUWEN_TEST("algorithm27", v.size() == solo.size())
        NUWEN_TEST("algorithm28", equal(v.begin(), v.end(), solo.begin()))
    }

    {
        const vector<thing> v = stable_nuke_dupes_copy(meow_thing);
        NUWEN_TEST("algorithm29", v.size() == solo.size())
        NUWEN_TEST("algorithm30", equal(v.begin(), v.end(), solo.begin()))
    }


    {
        vector<thing> v;

        copy_if(meow_thing.begin(), meow_thing.end(), back_inserter(v), &thing::all_caps);

        const vs_t correct = vec(glu<string>("FLUFFY")("CUTE")("KITTY"));

        NUWEN_TEST("algorithm31", v.size() == correct.size())
        NUWEN_TEST("algorithm32", equal(v.begin(), v.end(), correct.begin()))
    }

    const vs_t nuked = vec(glu<string>("Kitty")("cute")("fluffy")("Cute")("Fluffy")("kitty"));

    {
        vector<thing> v = meow_thing;

        nuke_if(v, &thing::all_caps);

        NUWEN_TEST("algorithm33", v.size() == nuked.size())
        NUWEN_TEST("algorithm34", equal(v.begin(), v.end(), nuked.begin()))
    }

    {
        const vector<thing> v = nuke_copy_if(meow_thing, &thing::all_caps);

        NUWEN_TEST("algorithm35", v.size() == nuked.size())
        NUWEN_TEST("algorithm36", equal(v.begin(), v.end(), nuked.begin()))
    }

    {
        vector<thing> v = meow_thing;

        for_each(v.begin(), v.end(), &thing::make_lower);

        const vs_t correct = vec(glu<string>("kitty")("fluffy")("cute")("cute")("fluffy")("cute")("kitty")("fluffy")("kitty"));

        NUWEN_TEST("algorithm37", v.size() == correct.size())
        NUWEN_TEST("algorithm38", equal(v.begin(), v.end(), correct.begin()))
    }

    NUWEN_TEST("algorithm39", *find_if(meow_thing.begin(), meow_thing.end(), &thing::all_caps) == "FLUFFY")
    NUWEN_TEST("algorithm40", count_if(meow_thing.begin(), meow_thing.end(), &thing::all_caps) == 3)

    {
        vector<string::size_type> v;

        transform(meow_thing.begin(), meow_thing.end(), back_inserter(v), &thing::size);

        NUWEN_TEST("algorithm41", v == vec(glu<string::size_type>(5)(6)(4)(4)(6)(4)(5)(6)(5)))
    }

    const vs_t replaced = vec(glu<string>("Kitty")("X")("cute")("X")("fluffy")("Cute")("X")("Fluffy")("kitty"));

    {
        vector<thing> v = meow_thing;

        replace_if(v.begin(), v.end(), &thing::all_caps, thing("X"));

        NUWEN_TEST("algorithm42", v.size() == replaced.size())
        NUWEN_TEST("algorithm43", equal(v.begin(), v.end(), replaced.begin()))
    }

    {
        vector<thing> v;

        replace_copy_if(meow_thing.begin(), meow_thing.end(), back_inserter(v), &thing::all_caps, thing("X"));

        NUWEN_TEST("algorithm44", v.size() == replaced.size())
        NUWEN_TEST("algorithm45", equal(v.begin(), v.end(), replaced.begin()))
    }

    {
        vector<thing> v = meow_thing;

        partition(v.begin(), v.end(), &thing::all_caps);

        vb_t caps;

        transform(v.begin(), v.end(), back_inserter(caps), &thing::all_caps);

        NUWEN_TEST("algorithm46", caps == vec(glu<bool>(1)(1)(1)(0)(0)(0)(0)(0)(0)))
    }

    {
        vector<thing> v = meow_thing;

        stable_partition(v.begin(), v.end(), &thing::all_caps);

        const vs_t correct = vec(glu<string>("FLUFFY")("CUTE")("KITTY")("Kitty")("cute")("fluffy")("Cute")("Fluffy")("kitty"));

        NUWEN_TEST("algorithm47", v.size() == correct.size())
        NUWEN_TEST("algorithm48", equal(v.begin(), v.end(), correct.begin()))
    }
}
