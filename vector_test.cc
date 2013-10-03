// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "gluon.hh"
#include "test.hh"
#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <string>
#include "external_end.hh"

using namespace std;
using namespace nuwen;
using namespace nuwen::pack;

int main() {
    {
        packed_bits pb;

        NUWEN_TEST("vector1", pb.vuc().empty())

        pb.push_back(1);
        pb.push_back(0);
        pb.push_back(1);

        NUWEN_TEST("vector2", pb.vuc() == vuc_t(1, 0xA0))

        pb.push_back(0);
        pb.push_back(1);
        pb.push_back(1);
        pb.push_back(0);
        pb.push_back(1);

        NUWEN_TEST("vector3", pb.vuc() == vuc_t(1, 0xAD))

        pb.push_back(0);

        NUWEN_TEST("vector4", pb.vuc() == vec(glu<uc_t>(0xAD)(0x00)))

        pb.push_back(1);

        NUWEN_TEST("vector5", pb.vuc() == vec(glu<uc_t>(0xAD)(0x40)))
    }

    NUWEN_TEST("vector6", bytes_from_bits(0) == 0)
    NUWEN_TEST("vector7", bytes_from_bits(1) == 1)
    NUWEN_TEST("vector8", bytes_from_bits(7) == 1)
    NUWEN_TEST("vector9", bytes_from_bits(8) == 1)
    NUWEN_TEST("vector10", bytes_from_bits(9) == 2)
    NUWEN_TEST("vector11", bytes_from_bits(137) == 18)
    NUWEN_TEST("vector12", bytes_from_bits(144) == 18)
    NUWEN_TEST("vector13", bytes_from_bits(145) == 19)

    {
        const vuc_t deadbeef = vec(glu<uc_t>(0xDE)(0xAD)(0xBE)(0xEF));

        NUWEN_TEST("vector14", bit_from_vuc(deadbeef, 0) == 1)
        NUWEN_TEST("vector15", bit_from_vuc(deadbeef, 1) == 1)
        NUWEN_TEST("vector16", bit_from_vuc(deadbeef, 2) == 0)
        NUWEN_TEST("vector17", bit_from_vuc(deadbeef, 7) == 0)
        NUWEN_TEST("vector18", bit_from_vuc(deadbeef, 8) == 1)
        NUWEN_TEST("vector19", bit_from_vuc(deadbeef, 9) == 0)
        NUWEN_TEST("vector20", bit_from_vuc(deadbeef, 31) == 1)
    }

    {
        const vul_t v(5, 0x12345678UL);
        const dul_t d(5, 0x12345678UL);

        NUWEN_TEST("vector21", sequence_cast<dul_t>(v) == d)
        NUWEN_TEST("vector22", sequence_cast<vul_t>(d) == v)
    }

    {
        const vs_t v = vec(glu<string>("cute")("fluffy")("kittens"));
        const ds_t d = deq(glu<string>("cute")("fluffy")("kittens"));
        const ls_t l = lst(glu<string>("cute")("fluffy")("kittens"));

        NUWEN_TEST("vector23", sequence_cast<ds_t>(v) == d)
        NUWEN_TEST("vector24", sequence_cast<vs_t>(d) == v)
        NUWEN_TEST("vector25", sequence_cast<ls_t>(v) == l)
        NUWEN_TEST("vector26", sequence_cast<vs_t>(l) == v)
        NUWEN_TEST("vector27", sequence_cast<ls_t>(d) == l)
        NUWEN_TEST("vector28", sequence_cast<ds_t>(l) == d)
    }

    {
        const string s("Meow");

        const vuc_t v = vec(glu<uc_t>('M')('e')('o')('w'));
        const duc_t d = deq(glu<uc_t>('M')('e')('o')('w'));
        const luc_t l = lst(glu<uc_t>('M')('e')('o')('w'));
        const vsc_t x = vec(glu<sc_t>('M')('e')('o')('w'));
        const vc_t  y = vec(glu<char>('M')('e')('o')('w'));

        NUWEN_TEST("vector29", string_cast<string>(v) == s)
        NUWEN_TEST("vector30", string_cast<string>(d) == s)
        NUWEN_TEST("vector31", string_cast<string>(l) == s)
        NUWEN_TEST("vector32", string_cast<string>(x) == s)
        NUWEN_TEST("vector33", string_cast<string>(y) == s)

        NUWEN_TEST("vector34", string_cast<vuc_t>(s) == v)
        NUWEN_TEST("vector35", string_cast<duc_t>(s) == d)
        NUWEN_TEST("vector36", string_cast<luc_t>(s) == l)
        NUWEN_TEST("vector37", string_cast<vsc_t>(s) == x)
        NUWEN_TEST("vector38", string_cast<vc_t >(s) == y)

        NUWEN_TEST("vector39", string_cast<string>(v) == "Meow")

        NUWEN_TEST("vector40", string_cast<vuc_t>("Meow") == v)
        NUWEN_TEST("vector41", string_cast<duc_t>("Meow") == d)
        NUWEN_TEST("vector42", string_cast<luc_t>("Meow") == l)
        NUWEN_TEST("vector43", string_cast<vsc_t>("Meow") == x)
        NUWEN_TEST("vector44", string_cast<vc_t >("Meow") == y)

        const char * const p = "Meow";

        NUWEN_TEST("vector45", string_cast<vuc_t>(p) == v)
    }

    NUWEN_TEST("vector46", uc_from_sc(-128) == 0)
    NUWEN_TEST("vector47", uc_from_sc(5) == 133)
    NUWEN_TEST("vector48", uc_from_sc(127) == 255)
    NUWEN_TEST("vector49", us_from_ss(-32768) == 0)
    NUWEN_TEST("vector50", us_from_ss(5) == 32773)
    NUWEN_TEST("vector51", us_from_ss(32767) == 65535)
    NUWEN_TEST("vector52", ul_from_sl(-2147483647L - 1) == 0)
    NUWEN_TEST("vector53", ul_from_sl(5) == 2147483653UL)
    NUWEN_TEST("vector54", ul_from_sl(2147483647) == 4294967295UL)
    NUWEN_TEST("vector55", ull_from_sll(-9223372036854775807LL - 1) == 0)
    NUWEN_TEST("vector56", ull_from_sll(5) == 9223372036854775813ULL)
    NUWEN_TEST("vector57", ull_from_sll(9223372036854775807LL) == 18446744073709551615ULL)
    NUWEN_TEST("vector58", sc_from_uc(0) == -128)
    NUWEN_TEST("vector59", sc_from_uc(133) == 5)
    NUWEN_TEST("vector60", sc_from_uc(255) == 127)
    NUWEN_TEST("vector61", ss_from_us(0) == -32768)
    NUWEN_TEST("vector62", ss_from_us(32773) == 5)
    NUWEN_TEST("vector63", ss_from_us(65535) == 32767)
    NUWEN_TEST("vector64", sl_from_ul(0) == -2147483647L - 1)
    NUWEN_TEST("vector65", sl_from_ul(2147483653UL) == 5)
    NUWEN_TEST("vector66", sl_from_ul(4294967295UL) == 2147483647)
    NUWEN_TEST("vector67", sll_from_ull(0) == -9223372036854775807LL - 1)
    NUWEN_TEST("vector68", sll_from_ull(9223372036854775813ULL) == 5)
    NUWEN_TEST("vector69", sll_from_ull(18446744073709551615ULL) == 9223372036854775807LL)

    {
        const vuc_t v = vec(glu<uc_t>(0xDE)(0xAD)(0xBE)(0xEF)(0x17)(0x76)(0x21)(0x61)(0x47));

        NUWEN_TEST("vector70", us_from_vuc(v) == 0xDEAD)
        NUWEN_TEST("vector71", us_from_vuc(v, 1) == 0xADBE)
        NUWEN_TEST("vector72", us_from_vuc(v, 7) == 0x6147)

        NUWEN_TEST("vector73", ul_from_vuc(v) == 0xDEADBEEFUL)
        NUWEN_TEST("vector74", ul_from_vuc(v, 1) == 0xADBEEF17UL)
        NUWEN_TEST("vector75", ul_from_vuc(v, 5) == 0x76216147UL)

        NUWEN_TEST("vector76", ull_from_vuc(v) == 0xDEADBEEF17762161ULL)
        NUWEN_TEST("vector77", ull_from_vuc(v, 1) == 0xADBEEF1776216147ULL)

        const vuc_ci_t i = v.begin() + 1;

        NUWEN_TEST("vector78", us_from_vuc(i, v.end()) == 0xADBE)
        NUWEN_TEST("vector79", ul_from_vuc(i, v.end()) == 0xADBEEF17UL)
        NUWEN_TEST("vector80", ull_from_vuc(i, v.end()) == 0xADBEEF1776216147ULL)

        const vuc_t a = vec(glu<uc_t>(0x21)(0x61));
        const vuc_t b = vec(glu<uc_t>(0x21)(0x61)(0x17)(0x76));
        const vuc_t c = vec(glu<uc_t>(0x21)(0x61)(0x17)(0x76)(0xDE)(0xAD)(0xBE)(0xEF));

        NUWEN_TEST("vector81", vuc_from_us(0x2161) == a)
        NUWEN_TEST("vector82", vuc_from_ul(0x21611776UL) == b)
        NUWEN_TEST("vector83", vuc_from_ull(0x21611776DEADBEEFULL) == c)
    }

    NUWEN_TEST("vector84", hex_from_uc(0x00) == "00")
    NUWEN_TEST("vector85", hex_from_uc(0x09) == "09")
    NUWEN_TEST("vector86", hex_from_uc(0x2F) == "2F")
    NUWEN_TEST("vector87", hex_from_uc(0xC4) == "C4")
    NUWEN_TEST("vector88", hex_from_uc(0xDE) == "DE")
    NUWEN_TEST("vector89", hex_from_uc(0xFF) == "FF")

    NUWEN_TEST("vector90", hex_from_vuc(vec(glu<uc_t>(0xFF)(0xDE)(0xC4)(0x2F)(0x09)(0x00))) == "FFDEC42F0900")

    {
        const string s("0f1e2d3c4b5a69788796A5B4C3D2E1F0AfBeCdDcEbFa");

        NUWEN_TEST("vector91", uc_from_hex("00") == 0x00)
        NUWEN_TEST("vector92", uc_from_hex("09") == 0x09)
        NUWEN_TEST("vector93", uc_from_hex("2F") == 0x2F)
        NUWEN_TEST("vector94", uc_from_hex("C4") == 0xC4)
        NUWEN_TEST("vector95", uc_from_hex("DE") == 0xDE)
        NUWEN_TEST("vector96", uc_from_hex("FF") == 0xFF)

        NUWEN_TEST("vector97", uc_from_hex(s) == 0x0F)
        NUWEN_TEST("vector98", uc_from_hex(s, 1) == 0xF1)
        NUWEN_TEST("vector99", uc_from_hex(s, 41) == 0xBF)
        NUWEN_TEST("vector100", uc_from_hex(s, 42) == 0xFA)

        const vuc_t v = vec(glu<uc_t>
            (0x0F)(0x1E)(0x2D)(0x3C)(0x4B)(0x5A)(0x69)(0x78)(0x87)(0x96)(0xA5)
            (0xB4)(0xC3)(0xD2)(0xE1)(0xF0)(0xAF)(0xBE)(0xCD)(0xDC)(0xEB)(0xFA));

        NUWEN_TEST("vector101", vuc_from_hex(s) == v)
    }
}
