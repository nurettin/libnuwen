// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "clock.hh"
#include "file.hh"
#include "gluon.hh"
#include "test.hh"
#include "typedef.hh"

#include "external_begin.hh"
    #include <iostream>
    #include <ostream>
    #include <string>
#include "external_end.hh"

using namespace std;
using namespace nuwen;
using namespace nuwen::chrono;
using namespace nuwen::file;

const string TEST_FILE("foobar.tmp");

bool test_create() {
    const vuc_t v(15000, 137);

    write_file(v, TEST_FILE);

    return read_file(TEST_FILE) == v;
}

bool test_overwrite() {
    const vuc_t v(12345, 47);

    write_file(v, TEST_FILE, overwrite);

    return read_file(TEST_FILE) == v;
}

bool test_append() {
    write_file(vuc_t(888, 144), TEST_FILE, append);

    return read_file(TEST_FILE) == vec(cat(vuc_t(12345, 47))(vuc_t(888, 144)));
}

bool test_remove() {
    const vuc_t v(pham::file::BLOCK_SIZE, 33);

    remove_file(TEST_FILE);

    write_file(v, TEST_FILE, create);

    return read_file(TEST_FILE) == v;
}

bool test_objects() {
    output_file out(TEST_FILE, overwrite);

    out.write(vuc_t(1000, 88));
    out.write(vuc_t(2000, 25));
    out.write(vuc_t(3000, 52));

    out.close();

    input_file in(TEST_FILE);

    const bool ret =
        in.read_at_most(1500) == vec(cat(vuc_t(1000, 88))(vuc_t(500, 25)))
        && in.read_rest() == vec(cat(vuc_t(1500, 25))(vuc_t(3000, 52)));

    in.close();

    remove_file(TEST_FILE);

    return ret;
}

bool test_speed(const string& filename) {
    watch w;
    const vuc_t& v = read_file(filename);
    const double read_time = w.seconds();

    w.reset();
    write_file(v, TEST_FILE);
    const double write_time = w.seconds();

    w.reset();
    const vuc_t& v2 = read_file(TEST_FILE);
    const double read2_time = w.seconds();

    w.reset();
    remove_file(TEST_FILE);
    const double remove_time = w.seconds();

    cout << "First Read (MB/s):  " << v.size() /  read_time / 1048576 << endl;
    cout << "Write (MB/s):       " << v.size() / write_time / 1048576 << endl;
    cout << "Second Read (MB/s): " << v.size() / read2_time / 1048576 << endl;
    cout << "Remove (ms):        " << remove_time * 1000              << endl;

    return v == v2;
}

int main(int argc, char * argv[]) {
    NUWEN_TEST("file1", test_create())
    NUWEN_TEST("file2", test_overwrite())
    NUWEN_TEST("file3", test_append())
    NUWEN_TEST("file4", test_remove())
    NUWEN_TEST("file5", test_objects())

    if (argc == 2) {
        NUWEN_TEST("file6", test_speed(argv[1]))
    }
}
