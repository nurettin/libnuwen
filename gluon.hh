// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_GLUON_HH
#define PHAM_GLUON_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "static_assert_private.hh"
#include "traits.hh"

#include "external_begin.hh"
    #include <cstddef>
    #include <deque>
    #include <list>
    #include <vector>
    #include <boost/mpl/if.hpp>
#include "external_end.hh"

namespace pham {
    namespace qcd {
        template <typename X, typename T> struct gluon_for;
        template <typename S, typename T> class sequence_gluon;
        template <typename T> class array_gluon;
    }
}

namespace nuwen {
    template <typename T, typename X> typename pham::qcd::gluon_for<X, T>::type glu(const X& x);

    template <typename T> pham::qcd::sequence_gluon<std::vector<T>, T> cat(const std::vector<T>& x);
    template <typename T> pham::qcd::sequence_gluon<std::deque<T>,  T> cat(const std::deque<T>&  x);
    template <typename T> pham::qcd::sequence_gluon<std::list<T>,   T> cat(const std::list<T>&   x);
    template <typename T, std::size_t N> pham::qcd::array_gluon<T>     cat(const T (&x)[N]);
}

namespace pham {
    namespace qcd {
        template <typename T> class gluon {
        public:
            explicit gluon(const gluon<T> * const prev) : m_prev(prev) { }

            template <typename X> typename gluon_for<X, T>::type operator()(const X& x) const;

            // A virtual destructor is not required, but to appease -Wnon-virtual-dtor,
            // I define one here anyways. This shouldn't hurt performance. Derived classes
            // repeat necessary virtual destructors as a reminder, but there's no point in
            // doing that for unnecessary virtual destructors.

            virtual ~gluon() { }

            template <typename C> void fill(C& c) const {
                if (m_prev) { m_prev->fill(c); }

                glom(c);
            }

        private:
            virtual void glom(std::vector<T>& v) const = 0;
            virtual void glom(std::deque<T>&  d) const = 0;
            virtual void glom(std::list<T>&   l) const = 0;

            const gluon<T> * const m_prev;
        };

        template <typename S, typename T> class sequence_gluon : public gluon<T> {
            PHAM_STATIC_ASSERT((nuwen::is_sequence_of<S, T>::value));

        public:
            sequence_gluon(const gluon<T> * const prev, const S& seq) : gluon<T>(prev), m_seq(seq) { }

        private:
            virtual void glom(std::vector<T>& v) const { v.insert(v.end(), m_seq.begin(), m_seq.end()); }
            virtual void glom(std::deque<T>&  d) const { d.insert(d.end(), m_seq.begin(), m_seq.end()); }
            virtual void glom(std::list<T>&   l) const { l.insert(l.end(), m_seq.begin(), m_seq.end()); }

            const S& m_seq;
        };

        template <typename T> class array_gluon : public gluon<T> {
        public:
            template <std::size_t N> array_gluon(const gluon<T> * const prev, const T (&a)[N]) : gluon<T>(prev), m_p(a), m_n(N) { }

        private:
            virtual void glom(std::vector<T>& v) const { v.insert(v.end(), m_p, m_p + m_n); }
            virtual void glom(std::deque<T>&  d) const { d.insert(d.end(), m_p, m_p + m_n); }
            virtual void glom(std::list<T>&   l) const { l.insert(l.end(), m_p, m_p + m_n); }

            const T * const   m_p;
            const std::size_t m_n;
        };

        template <typename Dst> struct cruncher {
            template <typename C, typename Src> static void crunch(C& c, const Src& s) {
                c.push_back(static_cast<const Dst&>(s));
            }
        };

        template <> struct cruncher<bool> {
            template <typename C, typename Src> static void crunch(C& c, const Src& s) {
                c.push_back(s != 0);
            }
        };

        template <typename E, typename T> class element_gluon : public gluon<T> {
            PHAM_STATIC_ASSERT((!nuwen::is_sequence_of<E, T>::value));

        public:
            element_gluon(const gluon<T> * const prev, const E& elem) : gluon<T>(prev), m_elem(elem) { }

        private:
            virtual void glom(std::vector<T>& v) const { cruncher<T>::crunch(v, m_elem); }
            virtual void glom(std::deque<T>&  d) const { cruncher<T>::crunch(d, m_elem); }
            virtual void glom(std::list<T>&   l) const { cruncher<T>::crunch(l, m_elem); }

            const E& m_elem;
        };

        template <typename X, typename T> struct gluon_for {
            typedef
                typename boost::mpl::if_<nuwen::is_sequence_of<X, T>, sequence_gluon<X, T>,
                    typename boost::mpl::if_<nuwen::is_bounded_array_of<X, T>, array_gluon<T>,
                        element_gluon<X, T>
                    >::type
                >::type type;
        };

        template <typename T> template <typename X> typename gluon_for<X, T>::type gluon<T>::operator()(const X& x) const {
            return typename gluon_for<X, T>::type(this, x);
        }

        template <typename C, typename T> C& operator+=(C& c, const gluon<T>& g) {
            g.fill(c);

            return c;
        }

        template <typename T> std::vector<T> vec(const gluon<T>& g) {
            std::vector<T> ret;

            return ret += g;
        }

        template <typename T> std::deque<T>  deq(const gluon<T>& g) {
            std::deque<T> ret;

            return ret += g;
        }

        template <typename T> std::list<T>   lst(const gluon<T>& g) {
            std::list<T> ret;

            return ret += g;
        }
    }
}

template <typename T, typename X> typename pham::qcd::gluon_for<X, T>::type nuwen::glu(const X& x) {
    return typename pham::qcd::gluon_for<X, T>::type(NULL, x);
}

template <typename T> pham::qcd::sequence_gluon<std::vector<T>, T> nuwen::cat(const std::vector<T>& x) {
    return pham::qcd::sequence_gluon<std::vector<T>, T>(NULL, x);
}

template <typename T> pham::qcd::sequence_gluon<std::deque<T>,  T> nuwen::cat(const std::deque<T>&  x) {
    return pham::qcd::sequence_gluon<std::deque<T>, T>(NULL, x);
}

template <typename T> pham::qcd::sequence_gluon<std::list<T>,   T> nuwen::cat(const std::list<T>&   x) {
    return pham::qcd::sequence_gluon<std::list<T>, T>(NULL, x);
}

template <typename T, std::size_t N> pham::qcd::array_gluon<T> nuwen::cat(const T (&x)[N]) {
    return pham::qcd::array_gluon<T>(NULL, x);
}

#endif // Idempotency
