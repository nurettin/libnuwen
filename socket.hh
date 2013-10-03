// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_SOCKET_HH
#define PHAM_SOCKET_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "clock.hh"
#include "gluon.hh"
#include "socket_private.hh"
#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <algorithm>
    #include <cstdlib>
    #include <exception>
    #include <map>
    #include <stdexcept>
    #include <string>
    #include <utility>
    #include <boost/lexical_cast.hpp>
    #include <boost/shared_ptr.hpp>
    #include <boost/utility.hpp>
#include "external_end.hh"

namespace pham {
    namespace sock {
        class proto_socket;
        struct cxn_state;
    }
}

namespace nuwen {
    inline void socket_startup();

    namespace sock {
        const ull_t DEFAULT_TIMEOUT_MS = 30000;
        const ul_t  DEFAULT_LIMIT      = 10485760;

        class client_socket {
        public:
            inline client_socket(const std::string& server, us_t port);

            inline vuc_t read(ul_t limit = DEFAULT_LIMIT);

            inline void write(const vuc_t& v);

        private:
            boost::shared_ptr<pham::sock::proto_socket> m_p;
        };

        class client_id {
        public:
            client_id() : m_id(INVALID_ID) { }

            bool operator==(const client_id& other) const { return m_id == other.m_id; }
            bool operator!=(const client_id& other) const { return m_id != other.m_id; }
            bool operator< (const client_id& other) const { return m_id <  other.m_id; }

        private:
            friend class server_socket;

            static const ull_t     INVALID_ID = 0;
            static const ull_t FIRST_VALID_ID = 1;

            explicit client_id(const ull_t id) : m_id(id) { }

            ull_t m_id;
        };

        class server_socket : public boost::noncopyable {
        public:
            inline explicit server_socket(us_t port,
                ull_t timeout_ms    = DEFAULT_TIMEOUT_MS,
                ul_t  receive_limit = DEFAULT_LIMIT,
                ul_t  send_limit    = DEFAULT_LIMIT);

            inline std::pair<client_id, vuc_t> next_request();

            inline void flush();

            inline void write_continue(client_id id, const vuc_t& v);
            inline void write_finish  (client_id id, const vuc_t& v);

            inline void finish(client_id id);

        private:
            inline void pump(std::pair<client_id, vuc_t> * request);

            inline void accept_client();

            typedef boost::shared_ptr<pham::sock::proto_socket> ptr_t;

            typedef std::map<client_id, boost::shared_ptr<pham::sock::cxn_state> > map_t;
            typedef map_t::iterator map_i_t;
            typedef map_t::const_iterator map_ci_t;

            const ptr_t m_server;
            map_t       m_clients;
            ull_t       m_next_id;
            const ull_t m_timeout_ms;
            const ul_t  m_receive_limit;
            const ul_t  m_send_limit;
        };
    }
}

namespace pham {
    inline void socket_cleanup() {
        #ifdef NUWEN_PLATFORM_WINDOWS
            if (socket_error(WSACleanup())) {
                std::terminate();
            }
        #endif
    }

    namespace sock {
        class proto_socket : public boost::noncopyable {
        public:
            explicit proto_socket(const PHAM_SOCKET s) : m_s(s) {
                if (invalid_socket(m_s)) {
                    throw std::runtime_error("RUNTIME ERROR: pham::sock::proto_socket::proto_socket() - Invalid socket.");
                }

                const int n = 1;

                if (socket_error(setsockopt(m_s, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char *>(&n), sizeof n))) {
                    shutdown_and_close();

                    throw std::runtime_error("RUNTIME ERROR: pham::sock::proto_socket::proto_socket() - setsockopt() failed.");
                }
            }

            PHAM_SOCKET raw() {
                return m_s;
            }

            nuwen::vuc_t primitive_read(const nuwen::ul_t n) {
                if (n == 0) {
                    throw std::logic_error("LOGIC ERROR: pham::sock::proto_socket::primitive_read() - Zero n.");
                }

                nuwen::vuc_t v(n);

                nuwen::ul_t bytesread = 0;

                while (bytesread < n) {
                    const int count = recv(m_s, reinterpret_cast<char *>(&v[bytesread]), static_cast<int>(n - bytesread), 0);

                    if (count <= 0) {
                        throw std::runtime_error("RUNTIME ERROR: pham::sock::proto_socket::primitive_read() - recv() failed.");
                    }

                    bytesread += count;
                }

                return v;
            }

            void primitive_write(const nuwen::vuc_t& v) {
                if (v.empty()) {
                    throw std::logic_error("LOGIC ERROR: pham::sock::proto_socket::primitive_write() - Empty v.");
                }

                nuwen::ul_t n = 0;

                while (n < v.size()) {
                    const int count = send(m_s, reinterpret_cast<const char *>(&v[n]), static_cast<int>(v.size() - n), 0);

                    if (count <= 0) {
                        throw std::runtime_error("RUNTIME ERROR: pham::sock::proto_socket::primitive_write() - send() failed.");
                    }

                    n += count;
                }
            }

            ~proto_socket() {
                shutdown_and_close();
            }

        private:
            void shutdown_and_close() {
                // The lack of error checking here is intended.

                #ifdef NUWEN_PLATFORM_WINDOWS
                    shutdown(m_s, SD_BOTH);
                    closesocket(m_s);
                #endif

                #ifdef NUWEN_PLATFORM_UNIX
                    shutdown(m_s, SHUT_RDWR);
                    close(m_s);
                #endif
            }

            const PHAM_SOCKET m_s;
        };

        struct address_information : public boost::noncopyable {
            address_information() : m_p(NULL) { }

            ~address_information() {
                if (m_p) {
                    freeaddrinfo(m_p);
                }
            }

            addrinfo * m_p;
        };

        struct cxn_state : public boost::noncopyable {
            cxn_state(const PHAM_SOCKET s, const nuwen::ull_t timeout_ms, const nuwen::ul_t receive_limit)
                : m_socket(s),
                  m_finished(false),
                  m_last_request_clock(nuwen::clock_ctr()),
                  m_timeout_ms(timeout_ms),
                  m_outgoing(),
                  m_incoming(),
                  m_length(sizeof m_length),
                  m_reading_length(true),
                  m_receive_limit(receive_limit) { }

            bool has_request() const {
                return !m_finished && !m_reading_length && m_incoming.size() == m_length;
            }

            nuwen::vuc_t extract_request() {
                if (!has_request()) {
                    throw std::logic_error("LOGIC ERROR: pham::sock::cxn_state::extract_request() - No request available.");
                }

                nuwen::vuc_t ret;

                m_incoming.swap(ret);

                m_last_request_clock = nuwen::clock_ctr();
                m_length = sizeof m_length;
                m_reading_length = true;

                return ret;
            }

            bool dead() const {
                return (m_finished && m_outgoing.empty())
                    || (nuwen::clock_ctr() - m_last_request_clock) * 1000 > nuwen::clock_freq() * m_timeout_ms;
            }

            bool receive_data(const wrapped_fd_set& readset) {
                if (readset.contains(m_socket.raw())) {
                    if (m_finished) {
                        throw std::logic_error("LOGIC ERROR: pham::sock::cxn_state::receive_data() - Shouldn't be finished.");
                    }

                    nuwen::vuc_t t(m_length - m_incoming.size());

                    if (t.empty()) {
                        throw std::logic_error("LOGIC ERROR: pham::sock::cxn_state::receive_data() - Empty t.");
                    }

                    const int count = recv(m_socket.raw(), reinterpret_cast<char *>(&t[0]), static_cast<int>(t.size()), 0);

                    if (count <= 0) {
                        return true;
                    }

                    t.resize(static_cast<nuwen::vuc_s_t>(count));

                    m_incoming += nuwen::cat(t);

                    if (m_reading_length && m_incoming.size() == m_length) {
                        m_reading_length = false;

                        m_length = nuwen::ul_from_vuc(m_incoming);

                        m_incoming.clear();

                        if (m_length > m_receive_limit) {
                            return true;
                        }
                    }
                }

                return false;
            }

            bool send_data(const wrapped_fd_set& writeset) {
                if (writeset.contains(m_socket.raw())) {
                    if (m_outgoing.empty()) {
                        throw std::logic_error("LOGIC ERROR: pham::sock::cxn_state::send_data() - Empty m_outgoing.");
                    }

                    const int count = send(m_socket.raw(), reinterpret_cast<const char *>(&m_outgoing[0]),
                        static_cast<int>(m_outgoing.size()), 0);

                    if (count <= 0) {
                        return true;
                    }

                    m_outgoing.erase(m_outgoing.begin(), m_outgoing.begin() + count);
                }

                return false;
            }

            proto_socket       m_socket;
            bool               m_finished;

            nuwen::ull_t       m_last_request_clock;
            const nuwen::ull_t m_timeout_ms;

            nuwen::vuc_t       m_outgoing;

            nuwen::vuc_t       m_incoming;
            nuwen::ul_t        m_length;
            bool               m_reading_length;
            const nuwen::ul_t  m_receive_limit;
        };
    }

    inline boost::shared_ptr<sock::proto_socket> make_socket() {
        const PHAM_SOCKET t = socket(PF_INET, SOCK_STREAM, 0);

        if (invalid_socket(t)) {
            throw std::runtime_error("RUNTIME ERROR: pham::make_socket() - socket() failed.");
        }

        boost::shared_ptr<sock::proto_socket> ret(new sock::proto_socket(t));

        return ret;
    }

    const int MAX_CLIENTS = 128;

    inline boost::shared_ptr<sock::proto_socket> make_server_socket(const nuwen::us_t port) {
        const boost::shared_ptr<sock::proto_socket> p = make_socket();

        const addrinfo hints = { AI_PASSIVE, PF_INET, SOCK_STREAM, 0, 0, NULL, NULL, NULL };

        sock::address_information ai;

        if (getaddrinfo(NULL, boost::lexical_cast<std::string>(port).c_str(), &hints, &ai.m_p) != 0) {
            throw std::runtime_error("RUNTIME ERROR: pham::make_server_socket() - getaddrinfo() failed.");
        }

        if (socket_error(bind(p->raw(), ai.m_p->ai_addr, static_cast<int>(ai.m_p->ai_addrlen)))) {
            throw std::runtime_error("RUNTIME ERROR: pham::make_server_socket() - bind() failed.");
        }

        if (socket_error(listen(p->raw(), MAX_CLIENTS))) {
            throw std::runtime_error("RUNTIME ERROR: pham::make_server_socket() - listen() failed.");
        }

        return p;
    }

    inline void wrapped_select(sock::wrapped_fd_set& readset, sock::wrapped_fd_set& writeset, const nuwen::ull_t timeout_ms) {
        if (readset.empty() && writeset.empty()) {
            throw std::logic_error("LOGIC ERROR: pham::wrapped_select() - Both sets are empty.");
        }

        const int n = static_cast<int>(std::max(readset.maximum(), writeset.maximum()) + 1);

        timeval timeout;
        timeout.tv_sec  = static_cast<nuwen::sl_t>( timeout_ms / 1000);
        timeout.tv_usec = static_cast<nuwen::sl_t>((timeout_ms % 1000) * 1000);

        if (socket_error(select(n, readset.ptr(), writeset.ptr(), NULL, &timeout))) {
            throw std::runtime_error("RUNTIME ERROR: pham::wrapped_select() - select() failed.");
        }
    }
}

inline void nuwen::socket_startup() {
    static bool s_already_called = false;

    if (s_already_called) {
        throw std::logic_error("LOGIC ERROR: nuwen::socket_startup() - Can't be called twice.");
    }

    s_already_called = true;

    #ifdef NUWEN_PLATFORM_WINDOWS
        const WORD desired_version = 0x0202;
        WSADATA info;

        if (WSAStartup(desired_version, &info) != 0) {
            throw std::runtime_error("RUNTIME ERROR: nuwen::socket_startup() - WSAStartup() failed.");
        }

        if (info.wVersion != desired_version) {
            pham::socket_cleanup();

            throw std::runtime_error("RUNTIME ERROR: nuwen::socket_startup() - Invalid version.");
        }

        if (std::atexit(pham::socket_cleanup) != 0) {
            pham::socket_cleanup();

            throw std::runtime_error("RUNTIME ERROR: nuwen::socket_startup() - std::atexit() failed.");
        }
    #endif
}

inline nuwen::sock::client_socket::client_socket(const std::string& server, const us_t port)
    : m_p(pham::make_socket()) {

    const addrinfo hints = { 0, PF_INET, SOCK_STREAM, 0, 0, NULL, NULL, NULL };

    pham::sock::address_information ai;

    if (getaddrinfo(server.c_str(), boost::lexical_cast<std::string>(port).c_str(), &hints, &ai.m_p) != 0) {
        throw std::runtime_error("RUNTIME ERROR: nuwen::sock::client_socket::client_socket() - getaddrinfo() failed.");
    }

    if (pham::socket_error(connect(m_p->raw(), ai.m_p->ai_addr, static_cast<int>(ai.m_p->ai_addrlen)))) {
        throw std::runtime_error("RUNTIME ERROR: nuwen::sock::client_socket::client_socket() - connect() failed.");
    }
}

inline nuwen::vuc_t nuwen::sock::client_socket::read(const ul_t limit) {
    const ul_t n = ul_from_vuc(m_p->primitive_read(4));

    if (n == 0) {
        return vuc_t();
    }

    if (n > limit) {
        throw std::runtime_error("RUNTIME ERROR: nuwen::sock::client_socket::read() - Huge n.");
    }

    return m_p->primitive_read(n);
}

inline void nuwen::sock::client_socket::write(const vuc_t& v) {
    m_p->primitive_write(vec(cat(vuc_from_ul(static_cast<ul_t>(v.size())))(v)));
}

inline nuwen::sock::server_socket::server_socket(const us_t port,
    const ull_t timeout_ms, const ul_t receive_limit, const ul_t send_limit)
    : m_server(pham::make_server_socket(port)),
      m_clients(),
      m_next_id(client_id::FIRST_VALID_ID),
      m_timeout_ms(timeout_ms),
      m_receive_limit(receive_limit),
      m_send_limit(send_limit) { }

inline std::pair<nuwen::sock::client_id, nuwen::vuc_t> nuwen::sock::server_socket::next_request() {
    std::pair<client_id, vuc_t> ret;

    pump(&ret);

    return ret;
}

inline void nuwen::sock::server_socket::flush() {
    pump(NULL);
}

inline void nuwen::sock::server_socket::write_continue(const client_id id, const vuc_t& v) {
    const map_i_t i = m_clients.find(id);

    if (i != m_clients.end()) {
        if (i->second->m_finished) {
            throw std::logic_error("LOGIC ERROR: nuwen::sock::server_socket::write_continue() - Shouldn't be finished.");
        }

        i->second->m_outgoing += cat(vuc_from_ul(static_cast<ul_t>(v.size())))(v);

        if (i->second->m_outgoing.size() > m_send_limit) {
            m_clients.erase(i);
        }
    }
}

inline void nuwen::sock::server_socket::write_finish(const client_id id, const vuc_t& v) {
    write_continue(id, v);
    finish(id);
}

inline void nuwen::sock::server_socket::finish(const client_id id) {
    const map_ci_t i = m_clients.find(id);

    if (i != m_clients.end()) {
        i->second->m_finished = true;
    }
}

inline void nuwen::sock::server_socket::pump(std::pair<client_id, vuc_t> * const request) {
    while (true) {
        // If a request is wanted and one exists, return it.
        if (request) {
            for (map_ci_t i = m_clients.begin(); i != m_clients.end(); ++i) {
                if (i->second->has_request()) {
                    *request = std::make_pair(i->first, i->second->extract_request());
                    return;
                }
            }
        }

        // Nuke dead clients.
        for (map_i_t i = m_clients.begin(); i != m_clients.end(); /* see body */) {
            if (i->second->dead()) {
                const map_i_t k = i++;
                m_clients.erase(k);
            } else {
                ++i;
            }
        }

        // Build up fd_sets.
        pham::sock::wrapped_fd_set readset;
        pham::sock::wrapped_fd_set writeset;

        // If a request is wanted and there is room, try to accept a new client.
        if (request && m_clients.size() < FD_SETSIZE) {
            readset.add(m_server->raw());
        }

        for (map_ci_t i = m_clients.begin(); i != m_clients.end(); ++i) {
            // If a request is wanted, try to receive data from all unfinished clients.
            if (request && !i->second->m_finished) {
                readset.add(i->second->m_socket.raw());
            }

            // Try to send data to all clients with data waiting to be sent.
            if (!i->second->m_outgoing.empty()) {
                writeset.add(i->second->m_socket.raw());
            }
        }

        // If we are flushing and no clients have data waiting to be sent, return.
        if (request == NULL && writeset.empty()) {
            return;
        }

        // Block until something interesting happens.
        pham::wrapped_select(readset, writeset, m_timeout_ms);

        // Receive data from and send data to all clients that are ready.
        for (map_i_t i = m_clients.begin(); i != m_clients.end(); /* see body */) {
            if (i->second->receive_data(readset) || i->second->send_data(writeset)) {
                const map_i_t k = i++;
                m_clients.erase(k);
            } else {
                ++i;
            }
        }

        // If we wanted a new client and one is available, accept it.
        if (readset.contains(m_server->raw())) {
            accept_client();
        }
    }
}

inline void nuwen::sock::server_socket::accept_client() {
    const PHAM_SOCKET t = accept(m_server->raw(), NULL, NULL);

    if (pham::invalid_socket(t)) {
        throw std::runtime_error("RUNTIME ERROR: nuwen::sock::server_socket::accept_client() - accept() failed.");
    }

    const boost::shared_ptr<pham::sock::cxn_state> p(new pham::sock::cxn_state(t, m_timeout_ms, m_receive_limit));

    m_clients.insert(std::make_pair(client_id(m_next_id++), p));
}

#endif // Idempotency
