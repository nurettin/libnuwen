// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#include "socket.hh"
#include "test.hh"
#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <iostream>
    #include <ostream>
    #include <stdexcept>
    #include <string>
    #include <boost/lexical_cast.hpp>
#include "external_end.hh"

using namespace std;
using namespace boost;
using namespace nuwen;
using namespace nuwen::sock;

bool test_startup() {
    socket_startup();
    return true;
}

void test_alice(client_socket& client) {
    client.write(string_cast<vuc_t>("Alice"));

    if (client.read() != string_cast<vuc_t>("Good morning, Alice.")) {
        throw runtime_error("RUNTIME ERROR: test_alice() - Incorrect greeting received.");
    }

    for (ul_t n = 0; n < 10000; ++n) {
        client.write(vuc_from_ul(n));

        const vuc_t v = client.read();

        if (v.size() != 4 || ul_from_vuc(v) != n * n) {
            throw runtime_error("RUNTIME ERROR: test_alice() - Incorrect response received.");
        }
    }

    if (client.read() != string_cast<vuc_t>("Bye, Alice.")) {
        throw runtime_error("RUNTIME ERROR: test_alice() - Incorrect farewell received.");
    }
}

void test_betty(client_socket& client) {
    client.write(string_cast<vuc_t>("Betty"));

    if (client.read() != string_cast<vuc_t>("Good afternoon, Betty.")) {
        throw runtime_error("RUNTIME ERROR: test_betty() - Incorrect greeting received.");
    }

    for (ul_t n = 0; n < 10000; ++n) {
        client.write(vuc_t(n, 0xAB));

        if (client.read() != vuc_t(n, 0xCD)) {
            throw runtime_error("RUNTIME ERROR: test_betty() - Incorrect response received.");
        }
    }

    if (client.read() != string_cast<vuc_t>("Later, Betty.")) {
        throw runtime_error("RUNTIME ERROR: test_betty() - Incorrect farewell received.");
    }
}

void test_carol(client_socket& client) {
    client.write(string_cast<vuc_t>("Carol"));

    if (client.read() != string_cast<vuc_t>("Good evening, Carol.")) {
        throw runtime_error("RUNTIME ERROR: test_carol() - Incorrect greeting received.");
    }

    for (ul_t n = 0; n < 10000; ++n) {
        client.write(string_cast<vuc_t>("foobar"));

        if (client.read() != string_cast<vuc_t>("baz")) {
            throw runtime_error("RUNTIME ERROR: test_carol() - Incorrect first response received.");
        }

        if (client.read() != string_cast<vuc_t>("quux")) {
            throw runtime_error("RUNTIME ERROR: test_carol() - Incorrect second response received.");
        }

        if (client.read() != string_cast<vuc_t>(lexical_cast<string>(n))) {
            throw runtime_error("RUNTIME ERROR: test_carol() - Incorrect third response received.");
        }
    }

    if (client.read() != string_cast<vuc_t>("Wait!")) {
        throw runtime_error("RUNTIME ERROR: test_carol() - Incorrect first farewell received.");
    }

    client.write(string_cast<vuc_t>("What?"));

    if (client.read() != string_cast<vuc_t>("Never mind.")) {
        throw runtime_error("RUNTIME ERROR: test_carol() - Incorrect second farewell received.");
    }
}

bool test_client(const int n) {
    if (n < 1 || n > 3) {
        throw runtime_error("RUNTIME ERROR: test_client() - Invalid n.");
    }

    client_socket client("localhost", 47123);

    if (n == 1) {
        test_alice(client);
    } else if (n == 2) {
        test_betty(client);
    } else {
        test_carol(client);
    }

    return true;
}

int main(int argc, char * argv[]) {
    if (argc != 2) {
        cout << "USAGE: While running socket_server_test, independently run these:" << endl;
        cout << "socket_client_test 1" << endl;
        cout << "socket_client_test 2" << endl;
        cout << "socket_client_test 3" << endl;
        cout << "And then press Enter in socket_server_test." << endl;

        return 0;
    }

    NUWEN_TEST("socket_client1", test_startup())
    NUWEN_TEST("socket_client2", test_client(lexical_cast<int>(argv[1])))
}
