// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_SOCKET_PRIVATE_HH
#define PHAM_SOCKET_PRIVATE_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "external_begin.hh"
    #include <algorithm>
    #include <stdexcept>
    #include <boost/utility.hpp>

    #ifdef NUWEN_PLATFORM_WINDOWS
        #include <winsock2.h>
        #include <ws2tcpip.h>
    #endif

    #ifdef NUWEN_PLATFORM_UNIX
        #include <netdb.h>
        #include <netinet/in.h>
        #include <netinet/tcp.h>
        #include <sys/select.h>
        #include <sys/socket.h>
        #include <sys/time.h>
        #include <sys/types.h>
        #include <unistd.h>
    #endif
#include "external_end.hh"

#ifdef NUWEN_PLATFORM_WINDOWS
    typedef SOCKET PHAM_SOCKET;
#endif

#ifdef NUWEN_PLATFORM_UNIX
    typedef int PHAM_SOCKET;
#endif

namespace pham {
    inline bool invalid_socket(const PHAM_SOCKET s) {
        return s == static_cast<PHAM_SOCKET>(-1);
    }

    inline bool socket_error(const int n) {
        #ifdef NUWEN_PLATFORM_WINDOWS
            return n == SOCKET_ERROR;
        #endif

        #ifdef NUWEN_PLATFORM_UNIX
            return n == -1;
        #endif
    }
}

#ifdef NUWEN_PLATFORM_GCC
    #pragma GCC system_header
#endif

namespace pham {
    namespace sock {
        class wrapped_fd_set : public boost::noncopyable {
        public:
            wrapped_fd_set() : m_dirty(false), m_empty(true), m_max(0) {
                FD_ZERO(&m_set);
            }

            bool contains(const PHAM_SOCKET s) const {
                return FD_ISSET(s, &m_set) != 0;
            }

            fd_set * ptr() {
                m_dirty = true;

                return m_empty ? NULL : &m_set;
            }

            void add(const PHAM_SOCKET s) {
                validate();

                FD_SET(s, &m_set);
                m_empty = false;
                m_max = std::max(s, m_max);
            }

            bool empty() const {
                validate();

                return m_empty;
            }

            PHAM_SOCKET maximum() const {
                validate();

                return m_max;
            }

        private:
            void validate() const {
                if (m_dirty) {
                    throw std::logic_error("LOGIC ERROR: pham::sock::wrapped_fd_set::validate() - ptr() invalidated this object.");
                }
            }

            fd_set      m_set;
            bool        m_dirty;
            bool        m_empty;
            PHAM_SOCKET m_max;
        };
    }
}

#endif // Idempotency
