// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_SERIAL_HH
#define PHAM_SERIAL_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "gluon.hh"
#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <deque>
    #include <list>
    #include <map>
    #include <set>
    #include <stdexcept>
    #include <string>
    #include <utility>
    #include <vector>
    #include <boost/type_traits/remove_const.hpp>
    #include <boost/utility.hpp>
#include "external_end.hh"

namespace nuwen {
    namespace ser {
        class serial : public boost::noncopyable {
        public:
            inline serial();

            template <typename T> explicit serial(const T& t);

            inline vuc_t vuc() const;

            template <typename T> serial& operator<<(const T& t);

            inline serial& operator<<(bool b);
            inline serial& operator<<(uc_t n);
            inline serial& operator<<(sc_t n);
            inline serial& operator<<(us_t n);
            inline serial& operator<<(ss_t n);
            inline serial& operator<<(ul_t n);
            inline serial& operator<<(sl_t n);
            inline serial& operator<<(ull_t n);
            inline serial& operator<<(sll_t n);
            inline serial& operator<<(const std::string& s);

            template <typename T> serial& operator<<(const std::vector<T>& v);
            template <typename T> serial& operator<<(const std::deque<T>& d);
            template <typename T> serial& operator<<(const std::list<T>& l);

            template <typename A, typename B> serial& operator<<(const std::pair<A, B>& p);
            template <typename T, typename L> serial& operator<<(const std::set<T, L>& s);
            template <typename T, typename L> serial& operator<<(const std::multiset<T, L>& ms);

            template <typename K, typename V, typename L> serial& operator<<(const std::map<K, V, L>& m);
            template <typename K, typename V, typename L> serial& operator<<(const std::multimap<K, V, L>& mm);

        private:
            template <typename C> serial& serialize_container(const C& c);

            vuc_t m_v;
        };
    }

    namespace des {
        class deserial : public boost::noncopyable {
        public:
            inline explicit deserial(const vuc_t& v);

            template <typename T> T get();

            inline vuc_ci_t curr() const;
            inline vuc_ci_t end() const;

            inline ull_t size() const;
            inline void consume(ull_t n);

        private:
            vuc_ci_t m_curr;
            vuc_ci_t m_end;
        };
    }
}

namespace pham {
    template <typename T> T deserialize_integer(nuwen::des::deserial& d) {
        if (d.size() < sizeof(T)) {
            throw std::runtime_error("RUNTIME ERROR: pham::deserialize_integer() - Not enough data.");
        }

        const T ret = t_from_vuc_unchecked<T>(d.curr());
        d.consume(sizeof(T));
        return ret;
    }

    template <typename S> S deserialize_sequence(nuwen::des::deserial& d) {
        S ret;

        const nuwen::ull_t n = d.get<nuwen::ull_t>();

        for (nuwen::ull_t i = 0; i < n; ++i) {
            ret.push_back(d.get<typename S::value_type>());
        }

        return ret;
    }

    template <typename U> U deserialize_uniq(nuwen::des::deserial& d) {
        U ret;

        const nuwen::ull_t n = d.get<nuwen::ull_t>();

        for (nuwen::ull_t i = 0; i < n; ++i) {
            if (!ret.insert(d.get<typename U::value_type>()).second) {
                throw std::runtime_error("RUNTIME ERROR: pham::deserialize_uniq() - Non-unique key encountered.");
            }
        }

        return ret;
    }

    template <typename E> E deserialize_eq(nuwen::des::deserial& d) {
        E ret;

        const nuwen::ull_t n = d.get<nuwen::ull_t>();

        for (nuwen::ull_t i = 0; i < n; ++i) {
            ret.insert(d.get<typename E::value_type>());
        }

        return ret;
    }

    template <typename T> struct des_helper {
        static T fxn(nuwen::des::deserial& d) { return T(d); }
    };

    template <typename T> struct des_helper<const T> {
        static const T fxn(nuwen::des::deserial& d) { return d.get<T>(); }
    };

    template <> struct des_helper<bool> {
        static bool fxn(nuwen::des::deserial& d) {
            const nuwen::uc_t n = d.get<nuwen::uc_t>();

            if (n > 1) {
                throw std::runtime_error("RUNTIME ERROR: pham::des_helper<bool>::fxn() - Invalid data.");
            }

            return n != 0;
        }
    };

    template <> struct des_helper<nuwen::uc_t> {
        static nuwen::uc_t fxn(nuwen::des::deserial& d) { return deserialize_integer<nuwen::uc_t>(d); }
    };

    template <> struct des_helper<nuwen::sc_t> {
        static nuwen::sc_t fxn(nuwen::des::deserial& d) { return nuwen::sc_from_uc(d.get<nuwen::uc_t>()); }
    };

    template <> struct des_helper<nuwen::us_t> {
        static nuwen::us_t fxn(nuwen::des::deserial& d) { return deserialize_integer<nuwen::us_t>(d); }
    };

    template <> struct des_helper<nuwen::ss_t> {
        static nuwen::ss_t fxn(nuwen::des::deserial& d) { return nuwen::ss_from_us(d.get<nuwen::us_t>()); }
    };

    template <> struct des_helper<nuwen::ul_t> {
        static nuwen::ul_t fxn(nuwen::des::deserial& d) { return deserialize_integer<nuwen::ul_t>(d); }
    };

    template <> struct des_helper<nuwen::sl_t> {
        static nuwen::sl_t fxn(nuwen::des::deserial& d) { return nuwen::sl_from_ul(d.get<nuwen::ul_t>()); }
    };

    template <> struct des_helper<nuwen::ull_t> {
        static nuwen::ull_t fxn(nuwen::des::deserial& d) { return deserialize_integer<nuwen::ull_t>(d); }
    };

    template <> struct des_helper<nuwen::sll_t> {
        static nuwen::sll_t fxn(nuwen::des::deserial& d) { return nuwen::sll_from_ull(d.get<nuwen::ull_t>()); }
    };

    template <> struct des_helper<std::string> {
        static std::string fxn(nuwen::des::deserial& d) { return nuwen::string_cast<std::string>(d.get<nuwen::vuc_t>()); }
    };

    template <typename T> struct des_helper<std::vector<T> > {
        static std::vector<T> fxn(nuwen::des::deserial& d) { return deserialize_sequence<std::vector<T> >(d); }
    };

    template <typename T> struct des_helper<std::deque<T> > {
        static std::deque<T> fxn(nuwen::des::deserial& d) { return deserialize_sequence<std::deque<T> >(d); }
    };

    template <typename T> struct des_helper<std::list<T> > {
        static std::list<T> fxn(nuwen::des::deserial& d) { return deserialize_sequence<std::list<T> >(d); }
    };

    template <typename A, typename B> struct des_helper<std::pair<A, B> > {
        static std::pair<A, B> fxn(nuwen::des::deserial& d) {
            const A a(d.get<typename boost::remove_const<A>::type>());
            const B b(d.get<typename boost::remove_const<B>::type>());

            return std::make_pair(a, b);
        }
    };

    template <typename T, typename L> struct des_helper<std::set<T, L> > {
        static std::set<T, L> fxn(nuwen::des::deserial& d) { return deserialize_uniq<std::set<T, L> >(d); }
    };

    template <typename T, typename L> struct des_helper<std::multiset<T, L> > {
        static std::multiset<T, L> fxn(nuwen::des::deserial& d) { return deserialize_eq<std::multiset<T, L> >(d); }
    };

    template <typename K, typename V, typename L> struct des_helper<std::map<K, V, L> > {
        static std::map<K, V, L> fxn(nuwen::des::deserial& d) { return deserialize_uniq<std::map<K, V, L> >(d); }
    };

    template <typename K, typename V, typename L> struct des_helper<std::multimap<K, V, L> > {
        static std::multimap<K, V, L> fxn(nuwen::des::deserial& d) { return deserialize_eq<std::multimap<K, V, L> >(d); }
    };
}

inline nuwen::ser::serial::serial() { }

template <typename T> nuwen::ser::serial::serial(const T& t) {
    *this << t;
}

inline nuwen::vuc_t nuwen::ser::serial::vuc() const {
    return m_v;
}

template <typename T> nuwen::ser::serial& nuwen::ser::serial::operator<<(const T& t) {
    return t.serialize(*this);
}

inline nuwen::ser::serial& nuwen::ser::serial::operator<<(const bool b) {
    return *this << static_cast<uc_t>(b);
}

inline nuwen::ser::serial& nuwen::ser::serial::operator<<(const uc_t n) {
    m_v.push_back(n);
    return *this;
}

inline nuwen::ser::serial& nuwen::ser::serial::operator<<(const sc_t n) {
    return *this << uc_from_sc(n);
}

inline nuwen::ser::serial& nuwen::ser::serial::operator<<(const us_t n) {
    m_v += cat(vuc_from_us(n));
    return *this;
}

inline nuwen::ser::serial& nuwen::ser::serial::operator<<(const ss_t n) {
    return *this << us_from_ss(n);
}

inline nuwen::ser::serial& nuwen::ser::serial::operator<<(const ul_t n) {
    m_v += cat(vuc_from_ul(n));
    return *this;
}

inline nuwen::ser::serial& nuwen::ser::serial::operator<<(const sl_t n) {
    return *this << ul_from_sl(n);
}

inline nuwen::ser::serial& nuwen::ser::serial::operator<<(const ull_t n) {
    m_v += cat(vuc_from_ull(n));
    return *this;
}

inline nuwen::ser::serial& nuwen::ser::serial::operator<<(const sll_t n) {
    return *this << ull_from_sll(n);
}

inline nuwen::ser::serial& nuwen::ser::serial::operator<<(const std::string& s) {
    return *this << string_cast<vuc_t>(s);
}

template <typename T> nuwen::ser::serial& nuwen::ser::serial::operator<<(const std::vector<T>& v) {
    return serialize_container(v);
}

template <typename T> nuwen::ser::serial& nuwen::ser::serial::operator<<(const std::deque<T>& d) {
    return serialize_container(d);
}

template <typename T> nuwen::ser::serial& nuwen::ser::serial::operator<<(const std::list<T>& l) {
    return serialize_container(l);
}

template <typename A, typename B> nuwen::ser::serial& nuwen::ser::serial::operator<<(const std::pair<A, B>& p) {
    return *this << p.first << p.second;
}

template <typename T, typename L> nuwen::ser::serial& nuwen::ser::serial::operator<<(const std::set<T, L>& s) {
    return serialize_container(s);
}

template <typename T, typename L> nuwen::ser::serial& nuwen::ser::serial::operator<<(const std::multiset<T, L>& ms) {
    return serialize_container(ms);
}

template <typename K, typename V, typename L> nuwen::ser::serial& nuwen::ser::serial::operator<<(const std::map<K, V, L>& m) {
    return serialize_container(m);
}

template <typename K, typename V, typename L> nuwen::ser::serial& nuwen::ser::serial::operator<<(const std::multimap<K, V, L>& mm) {
    return serialize_container(mm);
}

template <typename C> nuwen::ser::serial& nuwen::ser::serial::serialize_container(const C& c) {
    *this << static_cast<ull_t>(c.size());

    for (typename C::const_iterator i = c.begin(); i != c.end(); ++i) {
        *this << *i;
    }

    return *this;
}

inline nuwen::des::deserial::deserial(const vuc_t& v) : m_curr(v.begin()), m_end(v.end()) { }

template <typename T> T nuwen::des::deserial::get() {
    return pham::des_helper<T>::fxn(*this);
}

inline nuwen::vuc_ci_t nuwen::des::deserial::curr() const { return m_curr; }

inline nuwen::vuc_ci_t nuwen::des::deserial::end() const { return m_end; }

inline nuwen::ull_t nuwen::des::deserial::size() const { return static_cast<ull_t>(m_end - m_curr); }

inline void nuwen::des::deserial::consume(const ull_t n) {
    if (n > size()) {
        throw std::logic_error("LOGIC ERROR: nuwen::des::deserial::consume() - Invalid n.");
    }

    m_curr += static_cast<vuc_d_t>(n);
}

#endif // Idempotency
