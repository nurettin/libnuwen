// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "gluon.hh"
#include "serial.hh"
#include "test.hh"
#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <functional>
    #include <map>
    #include <set>
    #include <string>
    #include <utility>
    #include <vector>
#include "external_end.hh"

using namespace std;
using namespace nuwen;
using namespace nuwen::ser;
using namespace nuwen::des;

template <typename T> void test(const string& id, const T& t) {
    const vuc_t v = serial(t).vuc();

    deserial d(v);

    NUWEN_TEST(id + "a", d.get<T>() == t)
    NUWEN_TEST(id + "b", d.size() == 0)
}

namespace {
    class foo {
    public:
        foo(const vuc_t& v, const string& a, const string& b) : m_v(v), m_p(a, b) { }

        serial& serialize(serial& s) const { return s << m_v << m_p; }

        explicit foo(deserial& d) : m_v(d.get<vuc_t>()), m_p(d.get<pair<string, string> >()) { }

        bool operator==(const foo& rhs) const { return m_v == rhs.m_v && m_p == rhs.m_p; }

    private:
        vuc_t m_v;
        pair<string, string> m_p;
    };
}

int main() {
    const bool b = true;

    const uc_t uc = 0xFF;
    const sc_t sc = -60;
    const us_t us = 0xDDEE;
    const ss_t ss = -31234;
    const ul_t ul = 0x99AABBCCUL;
    const sl_t sl = -2123456789L;
    const ull_t ull = 0xDEADBEEF17762161ULL;
    const sll_t sll = -9123456789012345678LL;

    const string str("So high, so low, so many things to know.");

    const vb_t v = vec(glu<bool>(1)(1)(0)(0)(1)(0)(0)(1));
    const ds_t d = deq(glu<string>("Mary")("had")("a")("little")("lamb"));
    const lsc_t l = lst(glu<sc_t>(5)(0)(-5)(-128)(127)(47)(-11));

    const pair<string, us_t> p("UFP", 2161);

    set<string, greater<string> > s;

    s.insert("alan");
    s.insert("charles");
    s.insert("bjarne");
    s.insert("donald");

    multiset<ul_t, greater<ul_t> > ms;

    ms.insert(47);
    ms.insert(137);
    ms.insert(47);
    ms.insert(2161);
    ms.insert(137);
    ms.insert(47);

    map<us_t, string, greater<us_t> > m;

    m[1776] = "USA";
    m[1983] = "STL";
    m[2161] = "UFP";
    m[1701] = "NCC";

    multimap<string, us_t, greater<string> > mm;

    mm.insert(pair<string, us_t>("even",    2));
    mm.insert(pair<string, us_t>( "odd",    5));
    mm.insert(pair<string, us_t>("even", 1776));
    mm.insert(pair<string, us_t>( "odd", 2161));
    mm.insert(pair<string, us_t>( "odd",   13));
    mm.insert(pair<string, us_t>("even",   88));

    const foo f(vuc_from_hex("1234567890ABCDEF2161"), "fluffy", "kitty");

    const vector<foo> x(47, f);

    test("serial1", b);
    test("serial2", uc);
    test("serial3", sc);
    test("serial4", us);
    test("serial5", ss);
    test("serial6", ul);
    test("serial7", sl);
    test("serial8", ull);
    test("serial9", sll);
    test("serial10", str);
    test("serial11", v);
    test("serial12", d);
    test("serial13", l);
    test("serial14", p);
    test("serial15", s);
    test("serial16", ms);
    test("serial17", m);
    test("serial18", mm);
    test("serial19", f);
    test("serial20", x);

    serial quux;

    quux << b << uc << sc << us << ss << ul << sl << ull << sll << str << v << d << l << p << s << ms << m << mm << f << x;

    const vuc_t flat = quux.vuc();

    deserial baz(flat);

    NUWEN_TEST("serial21", baz.get<bool>() == b)
    NUWEN_TEST("serial22", baz.get<uc_t>() == uc)
    NUWEN_TEST("serial23", baz.get<sc_t>() == sc)
    NUWEN_TEST("serial24", baz.get<us_t>() == us)
    NUWEN_TEST("serial25", baz.get<ss_t>() == ss)
    NUWEN_TEST("serial26", baz.get<ul_t>() == ul)
    NUWEN_TEST("serial27", baz.get<sl_t>() == sl)
    NUWEN_TEST("serial28", baz.get<ull_t>() == ull)
    NUWEN_TEST("serial29", baz.get<sll_t>() == sll)
    NUWEN_TEST("serial30", baz.get<string>() == str)
    NUWEN_TEST("serial31", baz.get<vb_t>() == v)
    NUWEN_TEST("serial32", baz.get<ds_t>() == d)
    NUWEN_TEST("serial33", baz.get<lsc_t>() == l)
    NUWEN_TEST("serial34", (baz.get<pair<string, us_t> >() == p))
    NUWEN_TEST("serial35", (baz.get<set<string, greater<string> > >() == s))
    NUWEN_TEST("serial36", (baz.get<multiset<ul_t, greater<ul_t> > >() == ms))
    NUWEN_TEST("serial37", (baz.get<map<us_t, string, greater<us_t> > >() == m))
    NUWEN_TEST("serial38", (baz.get<multimap<string, us_t, greater<string> > >() == mm))
    NUWEN_TEST("serial39", baz.get<foo>() == f)
    NUWEN_TEST("serial40", baz.get<vector<foo> >() == x)

    NUWEN_TEST("serial41", baz.size() == 0)
}
