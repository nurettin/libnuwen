// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "gluon.hh"
#include "test.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <string>
    #include <vector>
#include "external_end.hh"

using namespace std;
using namespace nuwen;

int main() {
    {
        vuc_t x;
        x.push_back(1);
        x.push_back(2);
        x.push_back(4);
        x.push_back(8);

        vuc_t y;
        y.push_back(100);
        y.push_back(99);
        y.push_back(98);

        vuc_t z;
        z.push_back(34);
        z.push_back(55);
        z.push_back(89);

        vuc_t xyz;
        xyz.push_back(1);
        xyz.push_back(2);
        xyz.push_back(4);
        xyz.push_back(8);
        xyz.push_back(100);
        xyz.push_back(99);
        xyz.push_back(98);
        xyz.push_back(34);
        xyz.push_back(55);
        xyz.push_back(89);

        NUWEN_TEST("gluon1", vec(glu<uc_t>(x)) == x)
        NUWEN_TEST("gluon2", vec(glu<uc_t>(x)(y)(z)) == xyz)
        NUWEN_TEST("gluon3", vec(glu<uc_t>(1)(2)(4)(8)) == x)
        NUWEN_TEST("gluon4", vec(glu<uc_t>(x)(100)(99)(98)(z)) == xyz)
        NUWEN_TEST("gluon5", vec(glu<uc_t>(1)(2)(4)(8)(y)(34)(55)(89)) == xyz)

        NUWEN_TEST("gluon6", vec(cat(x)) == x)
        NUWEN_TEST("gluon7", vec(cat(x)(y)(z)) == xyz)
        NUWEN_TEST("gluon8", vec(cat(x)(100)(99)(98)(z)) == xyz)

        duc_t d(xyz.begin(), xyz.end());

        NUWEN_TEST("gluon9", deq(cat(x)(100)(99)(98)(z)) == d)

        luc_t l(xyz.begin(), xyz.end());

        NUWEN_TEST("gluon10", lst(cat(x)(100)(99)(98)(z)) == l)

        vuc_t a(3, 50);
        a.push_back(21);

        NUWEN_TEST("gluon11", vec(cat(duc_t(3, 50))(21)) == a)
        NUWEN_TEST("gluon12", vec(cat(luc_t(3, 50))(21)) == a)

        vector<vuc_t> m;
        m.push_back(x);
        m.push_back(y);
        m.push_back(z);

        NUWEN_TEST("gluon13", vec(glu<vuc_t>(x)(y)(z)) == m)
    }

    {
        vul_t taxi(100, 1729);

        NUWEN_TEST("gluon14", vec(cat(lul_t(22, 1729))(dul_t(33, 1729))(1729)(lul_t(11, 1729))(vul_t(33, 1729))) == taxi)
    }

    {
        vs_t v;
        v.push_back("cute");
        v.push_back("cute");
        v.push_back("cute");
        v.push_back("fluffy");
        v.push_back("kittens");

        const string k("kittens");

        NUWEN_TEST("gluon15", vec(cat(vs_t(3, "cute"))("fluffy")(k)) == v)
    }

    {
        const vb_t x = vec(cat(lb_t(2, true))(db_t(2, false))(true)(vb_t(2, false))(true)(0)(1)(vb_t(47, 0))(db_t(7, 1))(lb_t(6, 1)));
        const vb_t y = vec(glu<bool>(1)(1)(0)(0)(1)(0)(0)(1)(false)(true)(db_t(47, false))(lb_t(13, true)));

        NUWEN_TEST("gluon16", x == y)
    }

    {
        const vs_t v = vec(glu<string>("fluffy")("kitty"));
        const ds_t d = deq(glu<string>("evil")("puppy"));
        const ls_t l = lst(glu<string>("bouncy")("bunny"));
        const string s("weasel");

        vs_t a;

        a += cat(v)(d)(l);
        a += glu<string>("wicked")(s);

        ds_t b;

        b += cat(v)(d)(l);
        b += glu<string>("wicked")(s);

        ls_t c;

        c += cat(v)(d)(l);
        c += glu<string>("wicked")(s);

        const vs_t k = vec(glu<string>("fluffy")("kitty")("evil")("puppy")("bouncy")("bunny")("wicked")("weasel"));

        // Including vector.hh for sequence_cast would be awkward here.
        NUWEN_TEST("gluon17", a == k)
        NUWEN_TEST("gluon18", b == ds_t(k.begin(), k.end()))
        NUWEN_TEST("gluon19", c == ls_t(k.begin(), k.end()))
    }

    {
        const uc_t a[] = { 47, 137, 64, 32 };
        const uc_t b[] = { 1, 3, 7 };
        const uc_t c[] = { 81, 27, 9, 3, 1 };

        NUWEN_TEST("gluon20", vec(glu<uc_t>(a)(10)(20)) == vec(glu<uc_t>(47)(137)(64)(32)(10)(20)))
        NUWEN_TEST("gluon21", vec(cat(b)(30)(40)) == vec(glu<uc_t>(1)(3)(7)(30)(40)))
        NUWEN_TEST("gluon22", vec(glu<uc_t>(50)(60)(c)(70)(80)) == vec(glu<uc_t>(50)(60)(81)(27)(9)(3)(1)(70)(80)))
        NUWEN_TEST("gluon23", vec(cat(b)(c)(a)(b)) == vec(glu<uc_t>(1)(3)(7)(81)(27)(9)(3)(1)(47)(137)(64)(32)(1)(3)(7)))
    }
}
