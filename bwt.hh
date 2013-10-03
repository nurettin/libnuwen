// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_BWT_HH
#define PHAM_BWT_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <algorithm>
    #include <list>
    #include <stack>
    #include <stdexcept>
    #include <utility>
    #include <vector>
    #include <boost/tuple/tuple.hpp>
    #include <boost/utility.hpp>
#include "external_end.hh"

namespace nuwen {
    inline vuc_t bwt(const vuc_t& v);
    inline vuc_t unbwt(const vuc_t& v);
}

// Uncomment to enable internal logic checks that should never fire.
// #define PHAM_BWT_LOGIC_CHECKS

namespace pham {
    namespace ukk {
        // Let there be N bytes.
        // Infinity is the location of the sentinel, which is N.
        // The sentineled length is N + 1.
        // There are N + 1 leaves.
        // There are at most N + 1 nodes including root and bottom.

        const nuwen::vuc_s_t MIN_ALLOWED_SIZE = 1;
        const nuwen::vuc_s_t MAX_ALLOWED_SIZE = 512 * 1048576;

        typedef nuwen::us_t symbol_t;
        typedef nuwen::sl_t index_t;

        const symbol_t SENTINEL = 256;
        const symbol_t SIGMA = 257;
        const symbol_t FILLER = 0;


        class wrapped_text : public boost::noncopyable {
        public:
            explicit wrapped_text(const nuwen::vuc_t& v) : m_v(v), m_infinity(static_cast<index_t>(v.size())) { }

            index_t infinity() const { return m_infinity; }

            symbol_t operator[](const index_t i) const {
                if (i == m_infinity) {
                    return SENTINEL;
                } else if (i < 0) {
                    return static_cast<symbol_t>(-i - 1);
                } else {
                    return m_v[static_cast<nuwen::vuc_s_t>(i)];
                }
            }

        private:
            const nuwen::vuc_t& m_v;
            const index_t       m_infinity;
        };


        template <typename T> class flex_alloc : public boost::noncopyable {
        public:
            // There are at most *roughly* N hybrid_edges.
            // A hybrid_edge is 20 bytes.
            // Using N / 20 for K would result in roughly 1 N wasted space at most.
            // Using N / 200 for K reduces this to .1 N.
            // As the list and vector scheme incurs some overhead, a lower limit of 1000 is placed on K.

            explicit flex_alloc(const nuwen::vuc_s_t n)
                : m_k(std::max<nuwen::vuc_s_t>(n / 200, 1000)), m_lst(), m_next(NULL), m_end(NULL) { }

            T * make() {
                if (m_next == m_end) {
                    m_lst.push_back(std::vector<T>());
                    m_lst.back().resize(m_k);

                    m_next = &m_lst.back()[0];
                    m_end = m_next + m_k;
                }

                return m_next++;
            }

        private:
            const nuwen::vuc_s_t       m_k;
            std::list<std::vector<T> > m_lst;
            T *                        m_next;
            const T *                  m_end;
        };


        template <typename T> class hard_alloc : public boost::noncopyable {
        public:
            explicit hard_alloc(const nuwen::vuc_s_t n)
                : m_v(n), m_next(&m_v[0]), m_begin(&m_v[0]), m_end(m_begin + m_v.size()) { }

            T * make() {
                #ifdef PHAM_BWT_LOGIC_CHECKS
                    if (m_next == m_end) {
                        throw std::logic_error("LOGIC ERROR: pham::ukk::hard_alloc<T>::make() - Out of space.");
                    }
                #endif

                return m_next++;
            }

            bool contains(const T * const p) const {
                return p >= m_begin && p < m_end;
            }

        private:
            std::vector<T>  m_v;
            T *             m_next;
            const T * const m_begin;
            const T * const m_end;
        };


        // left and right are INCLUSIVE indices.

        // The first symbol of an edge is at the left index.

        // A node contains only a suffix link and an edge_list.
        // Ukkonen never travels the suffix links of leaves.
        // Leaves are never given any children.
        // Therefore, leaves contain no useful information.
        // Therefore, an edge to a leaf has a NULL child.

        struct edge {
            edge *  m_next;
            index_t m_left;

            edge *   next() const { return m_next; }
            index_t  left() const { return m_left; }
        };


        // The edges are kept sorted by their first symbols in increasing order.

        class edge_list {
        public:
            edge_list() : m_head(NULL) { }

            std::pair<edge *, edge *> find(const symbol_t a, const wrapped_text& text) const {
                edge * old = NULL;

                for (edge * p = m_head; p != NULL; old = p, p = p->next()) {
                    const symbol_t x = text[p->left()];

                    if (x < a) {
                        // Keep looking.
                    } else if (x == a) {
                        return std::make_pair(old, p); // Found it.
                    } else {
                        return std::pair<edge *, edge *>(old, NULL); // Stop looking.
                    }
                }

                return std::pair<edge *, edge *>(old, NULL); // We looked at everything and didn't find it.
            }

            bool exists(const symbol_t a, const wrapped_text& text) const {
                return find(a, text).second != NULL;
            }

            edge * get(const symbol_t a, const wrapped_text& text) const {
                edge * const p = find(a, text).second;

                #ifdef PHAM_BWT_LOGIC_CHECKS
                    if (p == NULL) {
                        throw std::logic_error("LOGIC ERROR: pham::ukk::edge_list::get() - Edge not found.");
                    }
                #endif

                return p;
            }

            void push(hard_alloc<edge>& leaf_alloc, const wrapped_text& text, const index_t left) {
                const symbol_t a = text[left];

                edge * old = NULL;

                for (edge * p = m_head; p != NULL; old = p, p = p->next()) {
                    const symbol_t x = text[p->left()];

                    if (x < a) {
                        // Keep looking.
                    } else {
                        #ifdef PHAM_BWT_LOGIC_CHECKS
                        // Case 1: Edit this edge.
                        if (x == a) {
                            throw std::logic_error("LOGIC ERROR: pham::ukk::edge_list::push() - Editing NULL, or non-NULL becoming NULL.");
                        }
                        #endif

                        // Case 2: Insert before this edge.
                        insert_between(leaf_alloc, old, p, left);
                        return;
                    }
                }

                // Case 3: Insert at the end.
                insert_between(leaf_alloc, old, NULL, left);
            }

            edge * head() const { return m_head; }

            void set_head(edge * const p) { m_head = p; }

        private:
            void insert_between(hard_alloc<edge>& leaf_alloc, edge * const old, edge * const p, const index_t left) {
                #ifdef PHAM_BWT_LOGIC_CHECKS
                    if (left < 0) {
                        throw std::logic_error("LOGIC ERROR: pham::ukk::edge_list::insert_between() - Negative leaf edge.");
                    }
                #endif

                edge * const nu = leaf_alloc.make();

                nu->m_next = p;
                nu->m_left = left;

                if (old) {
                    old->m_next = nu;
                } else {
                    m_head = nu;
                }
            }

            edge * m_head;
        };


        struct node {
            node() : m_edges(), m_link(NULL) { }

            edge_list m_edges;
            node *    m_link;
        };


        struct hybrid_edge {
            edge    m_first; // The 9.2/17 trick.
            index_t m_right;
            node    m_node;
        };


        class tree : public boost::noncopyable {
        public:
            explicit tree(const nuwen::vuc_t& v)
                : m_hybrid_alloc(v.size()), m_leaf_alloc(v.size() + 1), m_negative_alloc(SIGMA), m_root(), m_bottom(), m_text(v) {

                m_root.m_link = &m_bottom;

                // We build the list of negative edges in reverse order, from the end to the beginning.
                for (int j = SIGMA; j >= 1; --j) {
                    edge * const p = m_negative_alloc.make();

                    p->m_next = m_bottom.m_edges.head();
                    p->m_left = -j;

                    m_bottom.m_edges.set_head(p);
                }

                // Algorithm 2, Steps 4 - 8

                std::pair<node *, index_t> curr(&m_root, 0);

                for (index_t i = 0; i <= m_text.infinity(); ++i) {
                    curr = update(curr.first, curr.second, i);
                    curr = canonize(curr.first, curr.second, i);
                }
            }

            template <typename Functor> void dfs(Functor f) {
                typedef std::pair<edge *, index_t> pair_t;

                std::stack<pair_t, std::vector<pair_t> > stak; // Vector-based for speed.

                stak.push(std::make_pair(m_root.m_edges.head(), 0));

                while (!stak.empty()) {
                    edge * const e = stak.top().first;

                    if (e) {
                        stak.top().first = e->next();

                        const index_t len = stak.top().second + get_right(e) - e->left() + 1;

                        if (m_leaf_alloc.contains(e)) {
                            f(static_cast<nuwen::ul_t>(len));
                        } else {
                            stak.push(std::make_pair(get_child(e)->m_edges.head(), len));
                        }
                    } else {
                        stak.pop();
                    }
                }
            }

        private:
            index_t get_right(const edge * const p) const {
                if (m_leaf_alloc.contains(p)) {
                    return m_text.infinity();
                } else if (m_negative_alloc.contains(p)) {
                    return p->left();
                } else {
                    return reinterpret_cast<const hybrid_edge *>(p)->m_right;
                }
            }

            node * get_child(edge * const p) {
                if (m_leaf_alloc.contains(p)) {
                    return NULL;
                } else if (m_negative_alloc.contains(p)) {
                    return &m_root;
                } else {
                    return &reinterpret_cast<hybrid_edge *>(p)->m_node;
                }
            }

            // Ukkonen: e->left() = k'  get_right(e) = p'  get_child(e) = s'
            std::pair<node *, index_t> canonize(node * s, index_t k, const index_t p) {
                if (k <= p) {
                    edge * e = s->m_edges.get(m_text[k], m_text);

                    while (get_right(e) - e->left() <= p - k) {
                        k += get_right(e) - e->left() + 1;
                        s = get_child(e);

                        if (k <= p) {
                            e = s->m_edges.get(m_text[k], m_text);
                        }
                    }
                }

                return std::make_pair(s, k);
            }

            std::pair<bool, node *> test_and_split(node * const s, const index_t k, const index_t p, const symbol_t t) {
                if (k > p) {
                    return std::make_pair(s->m_edges.exists(t, m_text), s);
                }

                edge * old;
                edge * e;

                boost::tie(old, e) = s->m_edges.find(m_text[k], m_text);

                const index_t kprime = e->left();

                #ifdef PHAM_BWT_LOGIC_CHECKS
                    if (m_text[kprime] != m_text[k]) {
                        throw std::logic_error("LOGIC ERROR: pham::ukk::tree::test_and_split() - Symbol mismatch.");
                    }
                #endif

                if (t == m_text[kprime + p - k + 1]) {
                    return std::make_pair(true, s);
                }

                hybrid_edge * const nu_hybrid = m_hybrid_alloc.make();

                edge * const nu = reinterpret_cast<edge *>(nu_hybrid);

                if (old) {
                    old->m_next = nu;
                } else {
                    s->m_edges.set_head(nu);
                }

                nu->m_next = e->next();
                nu->m_left = kprime;
                nu_hybrid->m_right = kprime + p - k;
                node * const r = &nu_hybrid->m_node;

                // We can now modify e.
                e->m_next = NULL;
                e->m_left = kprime + p - k + 1;
                r->m_edges.set_head(e);

                return std::make_pair(false, r);
            }

            std::pair<node *, index_t> update(node * s, index_t k, const index_t i) {
                bool endpoint;
                node * r;
                node * oldr = &m_root;

                boost::tie(endpoint, r) = test_and_split(s, k, i - 1, m_text[i]);

                while (!endpoint) {
                    r->m_edges.push(m_leaf_alloc, m_text, i);

                    if (oldr != &m_root) {
                        oldr->m_link = r;
                    }

                    oldr = r;

                    boost::tie(s, k) = canonize(s->m_link, k, i - 1);

                    boost::tie(endpoint, r) = test_and_split(s, k, i - 1, m_text[i]);
                }

                if (oldr != &m_root) {
                    oldr->m_link = s;
                }

                return std::make_pair(s, k);
            }

            flex_alloc<hybrid_edge> m_hybrid_alloc;
            hard_alloc<edge>        m_leaf_alloc;
            hard_alloc<edge>        m_negative_alloc;
            node                    m_root;
            node                    m_bottom;
            const wrapped_text      m_text;
        };


        class bwt_helper {
        public:
            bwt_helper(const nuwen::vuc_t& src, nuwen::vuc_t& dest)
                : m_src(src.begin()), m_n(src.size()), m_dest(dest.begin() + 8), m_dest_orig(m_dest),
                m_primarydest(dest.begin()), m_sentineldest(dest.begin() + 4) { }

            void operator()(const nuwen::ul_t len) {
                if (len < m_n) {
                    *m_dest++ = m_src[static_cast<index_t>(m_n - len)];
                } else if (len == m_n) {
                    const nuwen::vuc_t primaryindex = nuwen::vuc_from_ul(static_cast<nuwen::ul_t>(m_dest - m_dest_orig));
                    std::copy(primaryindex.begin(), primaryindex.end(), m_primarydest);
                    *m_dest++ = m_src[0];
                } else {
                    const nuwen::vuc_t sentinelindex = nuwen::vuc_from_ul(static_cast<nuwen::ul_t>(m_dest - m_dest_orig));
                    std::copy(sentinelindex.begin(), sentinelindex.end(), m_sentineldest);
                    *m_dest++ = FILLER;
                }
            }

        private:
            nuwen::vuc_ci_t m_src;
            nuwen::vuc_s_t  m_n;
            nuwen::vuc_i_t  m_dest;
            nuwen::vuc_ci_t m_dest_orig;
            nuwen::vuc_i_t  m_primarydest;
            nuwen::vuc_i_t  m_sentineldest;
        };
    }
}

inline nuwen::vuc_t nuwen::bwt(const vuc_t& v) {
    using namespace std;
    using namespace pham::ukk;

    if (v.size() < MIN_ALLOWED_SIZE) {
        throw logic_error("LOGIC ERROR: nuwen::bwt() - v is too small.");
    }

    if (v.size() > MAX_ALLOWED_SIZE) {
        throw runtime_error("RUNTIME ERROR: nuwen::bwt() - v is too big.");
    }

    vuc_t ret(v.size() + 9);

    tree st(v);

    st.dfs(bwt_helper(v, ret));

    return ret;
}

inline nuwen::vuc_t nuwen::unbwt(const vuc_t& v) {
    using namespace std;
    using namespace pham::ukk;

    if (v.size() < 9 + MIN_ALLOWED_SIZE) {
        throw runtime_error("RUNTIME ERROR: nuwen::unbwt() - v is too small.");
    }

    if (v.size() > 9 + MAX_ALLOWED_SIZE) {
        throw runtime_error("RUNTIME ERROR: nuwen::unbwt() - v is too big.");
    }

    const ul_t primaryindex = ul_from_vuc(v, 0);
    const ul_t sentinelindex = ul_from_vuc(v, 4);
    const vuc_ci_t src = v.begin() + 8;
    const vuc_s_t n = v.size() - 8;

    if (primaryindex >= n) {
        throw runtime_error("RUNTIME ERROR: nuwen::unbwt() - Invalid primary index.");
    }

    if (sentinelindex >= n) {
        throw runtime_error("RUNTIME ERROR: nuwen::unbwt() - Invalid sentinel index.");
    }

    if (src[static_cast<index_t>(sentinelindex)] != FILLER) {
        throw runtime_error("RUNTIME ERROR: nuwen::unbwt() - Sentinel index doesn't contain filler.");
    }

    vul_t freqs(SIGMA, 0); // Fenwick's K

    for (vuc_s_t i = 0; i < n; ++i) {
        ++freqs[static_cast<vul_s_t>(i == sentinelindex ? SENTINEL : src[static_cast<index_t>(i)])];
    }

    vul_t mapping(SIGMA); // Fenwick's M

    mapping[0] = 0;
    for (ul_t i = 1; i < SIGMA; ++i) {
        mapping[i] = mapping[i - 1] + freqs[i - 1];
    }

    vul_t links(n); // Fenwick's L

    for (vuc_s_t i = 0; i < n; ++i) {
        links[i] = mapping[static_cast<vul_s_t>(i == sentinelindex ? SENTINEL : src[static_cast<index_t>(i)])]++;
    }

    vuc_t ret(n);

    ul_t index = primaryindex;

    for (vuc_ri_t i = ret.rbegin(); i != ret.rend(); ++i) {
        index = links[index];
        *i = src[static_cast<index_t>(index)];
    }

    if (ret.back() != FILLER) {
        throw runtime_error("RUNTIME ERROR: nuwen::unbwt() - ret's last byte isn't filler.");
    }

    ret.pop_back();

    return ret;
}

#undef PHAM_BWT_LOGIC_CHECKS

#endif // Idempotency
