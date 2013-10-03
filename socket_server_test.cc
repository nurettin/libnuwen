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
    #include <map>
    #include <ostream>
    #include <stdexcept>
    #include <string>
    #include <utility>
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

map<string, client_id> get_ids(server_socket& serv) {
    map<string, client_id> ret;

    for (int i = 0; i < 3; ++i) {
        const pair<client_id, vuc_t> p = serv.next_request();

        ret[string_cast<string>(p.second)] = p.first;
    }

    if (ret.size() != 3) {
        throw runtime_error("RUNTIME ERROR: get_ids() - Should have 3 clients.");
    }

    if (ret.find("Alice") == ret.end()) {
        throw runtime_error("RUNTIME ERROR: get_ids() - Alice not found.");
    }

    if (ret.find("Betty") == ret.end()) {
        throw runtime_error("RUNTIME ERROR: get_ids() - Betty not found.");
    }

    if (ret.find("Carol") == ret.end()) {
        throw runtime_error("RUNTIME ERROR: get_ids() - Carol not found.");
    }

    return ret;
}

bool test_server() {
    server_socket serv(47123);

    string dummy;
    getline(cin, dummy);

    const map<string, client_id> id_from_name = get_ids(serv);

    const client_id id_alice = id_from_name.find("Alice")->second;
    const client_id id_betty = id_from_name.find("Betty")->second;
    const client_id id_carol = id_from_name.find("Carol")->second;

    serv.write_continue(id_alice, string_cast<vuc_t>("Good morning, Alice."));
    serv.write_continue(id_betty, string_cast<vuc_t>("Good afternoon, Betty."));
    serv.write_continue(id_carol, string_cast<vuc_t>("Good evening, Carol."));

    ul_t n_alice = 0;
    ul_t n_betty = 0;
    ul_t n_carol = 0;

    for (int i = 0; i < 30000; ++i) {
        const pair<client_id, vuc_t> p = serv.next_request();

        if (p.first == id_alice) {
            if (p.second.size() != 4) {
                throw runtime_error("RUNTIME ERROR: test_server() - Alice should send 4-byte requests.");
            }

            if (ul_from_vuc(p.second) != n_alice) {
                throw runtime_error("RUNTIME ERROR: test_server() - Alice should send vuc_from_ul(N).");
            }

            serv.write_continue(id_alice, vuc_from_ul(n_alice * n_alice));

            ++n_alice;
        } else if (p.first == id_betty) {
            if (p.second != vuc_t(n_betty, 0xAB)) {
                throw runtime_error("RUNTIME ERROR: test_server() - Betty should send vuc_t(N, 0xAB).");
            }

            serv.write_continue(id_betty, vuc_t(n_betty, 0xCD));

            ++n_betty;
        } else if (p.first == id_carol) {
            if (p.second != string_cast<vuc_t>("foobar")) {
                throw runtime_error("RUNTIME ERROR: test_server() - Carol should send \"foobar\".");
            }

            serv.write_continue(id_carol, string_cast<vuc_t>("baz"));
            serv.write_continue(id_carol, string_cast<vuc_t>("quux"));
            serv.write_continue(id_carol, string_cast<vuc_t>(lexical_cast<string>(n_carol)));

            ++n_carol;
        } else {
            throw runtime_error("RUNTIME ERROR: test_server() - Unknown ID.");
        }
    }

    if (n_alice != 10000 || n_betty != 10000 || n_carol != 10000) {
        throw runtime_error("RUNTIME ERROR: test_server() - Bad number of requests.");
    }

    serv.write_continue(id_alice, string_cast<vuc_t>("Bye, Alice."));
    serv.finish(id_alice);

    serv.write_finish(id_betty, string_cast<vuc_t>("Later, Betty."));

    serv.write_continue(id_carol, string_cast<vuc_t>("Wait!"));

    const pair<client_id, vuc_t> p = serv.next_request();

    if (p.first != id_carol) {
        throw runtime_error("RUNTIME ERROR: test_server() - Carol should respond here.");
    }

    if (p.second != string_cast<vuc_t>("What?")) {
        throw runtime_error("RUNTIME ERROR: test_server() - Incorrect response from Carol.");
    }

    serv.write_finish(id_carol, string_cast<vuc_t>("Never mind."));

    serv.flush();

    return true;
}

int main() {
    cout << "USAGE: While running socket_server_test, independently run these:" << endl;
    cout << "socket_client_test 1" << endl;
    cout << "socket_client_test 2" << endl;
    cout << "socket_client_test 3" << endl;
    cout << "And then press Enter in socket_server_test." << endl;

    NUWEN_TEST("socket_server1", test_startup())
    NUWEN_TEST("socket_server2", test_server())
}
