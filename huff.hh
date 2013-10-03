// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_HUFF_HH
#define PHAM_HUFF_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

// LJM developed the ideas behind the automata.

#ifdef PHAM_AUTOMATON_TIMING
    #include "clock.hh"
#endif

#include "gluon.hh"
#include "typedef.hh"
#include "vector.hh"

#include "external_begin.hh"
    #include <algorithm>
    #include <numeric>
    #include <queue>
    #include <stdexcept>
    #include <vector>
    #include <boost/utility.hpp>
#include "external_end.hh"

namespace nuwen {
    inline vuc_t huff(const vuc_t& v);
    inline vuc_t puff(const vuc_t& v);
}

namespace pham {
    namespace huff {
        typedef nuwen::vuc_s_t freq_t;
        typedef std::vector<freq_t> vfreq_t;


        class node {
        public:
            node() : m_freq(0), m_byte(0), m_left(NULL), m_right(NULL) { }

            node(const freq_t f, const nuwen::uc_t b)
                : m_freq(f), m_byte(b), m_left(NULL), m_right(NULL) { }

            // The m_byte of internal nodes is ignored.
            node(const node * const l, const node * const r)
                : m_freq(l->m_freq + r->m_freq), m_byte(0), m_left(l), m_right(r) { }

            freq_t        freq() const { return m_freq;  }
            nuwen::uc_t   byte() const { return m_byte;  }
            const node *  left() const { return m_left;  }
            const node * right() const { return m_right; }

            void set_left( const node * const l) { m_left  = l; }
            void set_right(const node * const r) { m_right = r; }

        private:
            freq_t       m_freq;
            nuwen::uc_t  m_byte;
            const node * m_left;
            const node * m_right;
        };

        inline void read_codelengths_helper(nuwen::vuc_t& v, const node * const p, const nuwen::uc_t h) {
            if (p->left()) {
                read_codelengths_helper(v, p->left(),  static_cast<nuwen::uc_t>(h + 1));
                read_codelengths_helper(v, p->right(), static_cast<nuwen::uc_t>(h + 1));
            } else {
                v[p->byte()] = h;
            }
        }

        inline nuwen::vuc_t read_codelengths(const node * const root) {
            nuwen::vuc_t ret(256);
            read_codelengths_helper(ret, root, 0);
            return ret;
        }

        struct comparator {
            bool operator()(const node * const l, const node * const r) const {
                return l->freq() > r->freq();
            }
        };


        inline vfreq_t frequencies(const nuwen::vuc_t& v) {
            vfreq_t freqs(256, 0);

            for (nuwen::vuc_ci_t i = v.begin(); i != v.end(); ++i) {
                ++freqs[*i];
            }

            return freqs;
        }

        inline nuwen::vuc_t make_codelengths(const vfreq_t& freqs) {
            std::vector<node> nodes;
            nodes.reserve(511);

            std::priority_queue<const node *, std::vector<const node *>, comparator> pq;

            for (nuwen::us_t i = 0; i < 256; ++i) {
                nodes.push_back(node(freqs[i], static_cast<nuwen::uc_t>(i)));
                pq.push(&nodes.back());
            }

            while (pq.size() > 1) {
                const node * const first = pq.top();
                pq.pop();
                const node * const second = pq.top();
                pq.pop();

                nodes.push_back(node(first, second));
                pq.push(&nodes.back());
            }

            return read_codelengths(pq.top());
        }

        inline nuwen::vuc_t make_codes(const nuwen::vuc_t& codelengths) {
            nuwen::vuc_t codes(256);

            nuwen::uc_t start = 0;

            for (int i = 255; i > 0; --i) {             // Fill in codes that have codelength i.
                for (nuwen::us_t j = 0; j < 256; ++j) {
                    if (codelengths[j] == i) {
                        codes[j] = start++;
                    }
                }

                start = static_cast<nuwen::uc_t>(start >> 1);
            }

            return codes;
        }


        // Despite occupying 128 KB, this results in a massive speedup.
        class byte_decoder : public boost::noncopyable {
        public:
            static const nuwen::us_t INVALID = 256;

            byte_decoder(const nuwen::vuc_t& codelengths, const nuwen::vuc_t& codes) {
                for (int i = 0; i < 256; ++i) {
                    for (int j = 0; j < 256; ++j) {
                        m_table[i][j] = INVALID;
                    }
                }

                for (nuwen::us_t i = 0; i < 256; ++i) {
                    m_table[codelengths[i]][codes[i]] = i;
                }
            }

            nuwen::us_t operator()(const nuwen::uc_t codelength, const nuwen::uc_t code) const {
                return m_table[codelength][code];
            }

        private:
            nuwen::us_t m_table[256][256];
        };


        inline const node * regen_tree_helper(std::vector<node>& nodes, const byte_decoder& decoder, nuwen::uc_t length, nuwen::uc_t code) {
            nodes.push_back(node());

            node * const p = &nodes.back();

            ++length;
            code = static_cast<nuwen::uc_t>(code << 1);

            const nuwen::us_t leftbyte = decoder(length, code);

            if (leftbyte == byte_decoder::INVALID) {
                p->set_left(regen_tree_helper(nodes, decoder, length, code));
            } else {
                p->set_left(&nodes[leftbyte]);
            }

            code |= 1;

            const nuwen::us_t rightbyte = decoder(length, code);

            if (rightbyte == byte_decoder::INVALID) {
                p->set_right(regen_tree_helper(nodes, decoder, length, code));
            } else {
                p->set_right(&nodes[rightbyte]);
            }

            return p;
        }

        // Given codelengths and codes, regenerates the Huffman tree and places it into nodes.
        // nodes will contain 511 elements.
        // The first 256 elements will be the leaf nodes. The leaf node for byte B will be at index B.
        // The next 255 elements will be the internal nodes, starting with the root (at index 256).
        // (All nodes will have zero frequencies; this information has been lost and doesn't matter.)
        // Returns a pointer to the root.
        inline const node * regen_tree(std::vector<node>& nodes, const nuwen::vuc_t& codelengths, const nuwen::vuc_t& codes) {
            if (!nodes.empty()) {
                throw std::logic_error("LOGIC ERROR: pham::huff::regen_tree() - nodes should be empty.");
            }

            nodes.reserve(511);

            for (int i = 0; i < 256; ++i) {
                nodes.push_back(node(0, static_cast<nuwen::uc_t>(i)));
            }

            const byte_decoder decoder(codelengths, codes);

            return regen_tree_helper(nodes, decoder, 0, 0);
        }


        inline nuwen::vuc_t huff_bits(const nuwen::vuc_t& v) {
            using namespace nuwen;

            const vfreq_t freqs       = frequencies(v);
            const vuc_t   codelengths = make_codelengths(freqs);
            const vuc_t   codes       = make_codes(codelengths);

            pack::packed_bits encoded;

            for (vuc_ci_t i = v.begin(); i != v.end(); ++i) {
                const uc_t byte   = *i;
                const uc_t code   = codes[byte];
                      uc_t length = codelengths[byte];

                while (length > 8) {
                    --length;
                    encoded.push_back(0);
                }

                while (length > 0) {
                    --length;
                    encoded.push_back(code >> length & 1);
                }
            }

            return vec(cat(codelengths)(encoded.vuc()));
        }

        inline nuwen::vuc_t puff_bits(const nuwen::vuc_t& v) {
            using namespace nuwen;

            const vuc_t codelengths(v.begin(), v.begin() + 256);
            const vuc_t codes = make_codes(codelengths);

            const byte_decoder decoder(codelengths, codes);

            vuc_t decompressed;

            uc_t mrs8b = 0; // Most Recently Seen 8 Bits
            uc_t nbs   = 0; // Number Of Bits Seen

            for (ull_t i = 2048; i < static_cast<ull_t>(v.size()) * 8; ++i) {
                mrs8b = static_cast<uc_t>(mrs8b << 1);                 // Make space for another bit.
                mrs8b = static_cast<uc_t>(mrs8b | bit_from_vuc(v, i)); // Load in another bit.
                ++nbs;                                                 // We've seen another bit.

                const us_t byte = decoder(nbs, mrs8b);

                if (byte != byte_decoder::INVALID) {
                    decompressed.push_back(static_cast<uc_t>(byte));
                    mrs8b = 0;
                    nbs = 0;
                }
            }

            return decompressed;
        }


        class huff_automaton : public boost::noncopyable {
        private:
            struct entry {
                nuwen::uc_t numemit;     // The number of bytes to output, [0, 6 or 12 or 32]. (See end of class.)
                nuwen::uc_t new_numbits; // The new number of compressed bits not yet output, [0, 7].
                nuwen::uc_t firstpart;   // The high byte of the shifted code.
                nuwen::uc_t secondpart;  // The low byte of the shifted code.
            };

            // When numemit is 0:         Emit nothing.                              Then m_bits |= secondpart.
            // When numemit is 1:         Emit m_bits | firstpart.                   Then m_bits = secondpart.
            // When numemit is 2 or more: Emit m_bits, numemit - 2 zeros, firstpart. Then m_bits = secondpart.

            entry          m_table[256][8]; // Indexed by byte, m_numbits.
            nuwen::uc_t    m_bits;          // LEFT ALIGNED compressed bits not yet output.
            nuwen::uc_t    m_numbits;       // The number of compressed bits not yet output, [0, 7].
            nuwen::vuc_i_t m_dest;          // Output destination.

        public:
            huff_automaton(const nuwen::vuc_t& codelengths, const nuwen::vuc_t& codes, const nuwen::vuc_i_t dest)
                : m_bits(0), m_numbits(0), m_dest(dest) {

                using namespace nuwen;

                for (nuwen::us_t possible_byte = 0; possible_byte < 256; ++possible_byte) {
                    const uc_t length = codelengths[possible_byte];
                    const uc_t code = codes[possible_byte];

                    for (int possible_numbits = 0; possible_numbits < 8; ++possible_numbits) {
                        entry * const ent = &m_table[possible_byte][possible_numbits];

                        // The total number of bits we have to deal with, [1, 51 or 97 or 262].
                        const us_t total_bits = static_cast<us_t>(possible_numbits + length);

                        ent->numemit     = static_cast<uc_t>(total_bits / 8);
                        ent->new_numbits = static_cast<uc_t>(total_bits % 8);

                        // The number of empty bits at the right of m_bits, [1, 8].
                        const uc_t empty_bits = static_cast<uc_t>(8 - ent->new_numbits);

                        // We break the code into two parts on a byte boundary.
                        const us_t shiftedcode = static_cast<us_t>(code << empty_bits);

                        ent->firstpart = static_cast<uc_t>(shiftedcode >> 8);
                        ent->secondpart = static_cast<uc_t>(shiftedcode & 0xFF);
                    }
                }
            }

            void operator()(nuwen::vuc_ci_t byte, const nuwen::vuc_ci_t end) {
                for (; byte != end; ++byte) {
                    const entry * const p = &m_table[*byte][m_numbits];
                    m_numbits = p->new_numbits;

                    switch (p->numemit) {
                        case 0:
                            m_bits = static_cast<nuwen::uc_t>(m_bits | p->secondpart);
                            break;
                        case 1:
                            *m_dest++ = static_cast<nuwen::uc_t>(m_bits | p->firstpart);
                            m_bits = p->secondpart;
                            break;
                        case 2:
                            *m_dest++ = m_bits;
                            *m_dest++ = p->firstpart;
                            m_bits = p->secondpart;
                            break;
                        default:
                            *m_dest = m_bits;
                            m_dest += p->numemit - 1;
                            *m_dest++ = p->firstpart;
                            m_bits = p->secondpart;
                            break;
                    }
                }

                if (m_numbits != 0) {
                    *m_dest++ = m_bits;
                }
            }

            // For inputs of less than 2^32 bytes, the maximum possible codelength is 44.
            // For inputs of less than 2^64 bytes, the maximum possible codelength is 90.
            // In the completely general case, the maximum possible codelength is 255.

            // We may have 7 compressed bits not yet output when we encounter a byte with the maximum possible codelength.
            // That gives us 51 or 97 or 262 bits, which is 6 or 12 or 32 full bytes to emit.

            // These upper bounds are nice to know, but aren't actually helpful.
        };

        inline nuwen::vuc_t huff_auto(const nuwen::vuc_t& v
            #ifdef PHAM_AUTOMATON_TIMING
                , double * const ctor_time = NULL
            #endif
        ) {
            using namespace nuwen;

            const vfreq_t freqs       = frequencies(v);
            const vuc_t   codelengths = make_codelengths(freqs);
            const vuc_t   codes       = make_codes(codelengths);

            const ull_t bits = std::inner_product(freqs.begin(), freqs.end(), codelengths.begin(), static_cast<ull_t>(0));

            vuc_t ret(static_cast<vuc_s_t>(256 + bytes_from_bits(bits)), 0);

            std::copy(codelengths.begin(), codelengths.end(), ret.begin());

            #ifdef PHAM_AUTOMATON_TIMING
                const nuwen::chrono::watch w;
            #endif

            huff_automaton automaton(codelengths, codes, ret.begin() + 256);

            #ifdef PHAM_AUTOMATON_TIMING
                if (ctor_time) {
                    *ctor_time = w.seconds();
                }
            #endif

            automaton(v.begin(), v.end());

            return ret;
        }


        class puff_automaton : public boost::noncopyable {
        private:
            // We begin each step on one of the 255 internal nodes,
            // reading any of the 256 possible compressed bytes.
            // The main table has an entry for what we should do.

            struct entry {
                nuwen::uc_t aux_id;       // See below.
                nuwen::uc_t first_symbol; // The first symbol to emit.
                nuwen::uc_t dest_node;    // The index of the internal node that we land on.
            };

            static const nuwen::uc_t ID_EMIT_NO_SYMBOLS = 254;
            static const nuwen::uc_t ID_EMIT_ONE_SYMBOL = 255;

            // Each step will emit [0, 8] symbols.
            // When the number of symbols to emit is 0, aux_id will be ID_EMIT_NO_SYMBOLS and first_symbol will be ignored.
            // When the number of symbols to emit is 1, aux_id will be ID_EMIT_ONE_SYMBOL and first_symbol will be used.
            // When the number of symbols to emit is [2, 8]:
            // After emitting the first symbol, we return to the root. This puts us into a known state. What happens next
            // (i.e. which other symbols we emit, as well as which internal node we finally land on) is determined solely
            // by the remaining bits of the compressed byte.
            // There could be as many as 7 bits (when the first bit causes us to emit the first symbol), or as few as 1.
            // (If there are 0 bits remaining when we emit the first symbol, we definitely won't emit a second symbol.)
            // 2^7 + 2^6 + ... + 2^1 = 254, so we will construct an ID in [0, 254) which corresponds to the remaining bits
            // of the compressed byte. This ID will be stored in aux_id and used to index into auxiliary tables with 254
            // entries that tell us what happens next.
            // We build the auxiliary tables first in order to speed up the construction of the main table. When building
            // the main table, as soon as we emit one symbol, we can look up what happens next and thereby avoid doing any
            // unnecessary computation.
            // m_aux_numemit[id] says how many more symbols will be emitted after the first. It can be any value in [0, 7].
            // m_aux_table[id][N] says what the Nth additional symbol is.
            // aux_dest[id] says what internal node we finally land on; it is used during construction of the main table.

            entry         m_main_table[255][256]; // Indexed by m_curr_node, byte. See above.
            nuwen::uc_t   m_aux_numemit[254];     // Indexed by aux_id. See above.
            nuwen::uc_t   m_aux_table[254][7];    // Indexed by aux_id. See above.
            nuwen::uc_t   m_curr_node;            // The index of the internal node that we are currently on.
            nuwen::vuc_t& m_dest;                 // Output destination.

        public:
            puff_automaton(const nuwen::vuc_t& codelengths, const nuwen::vuc_t& codes, nuwen::vuc_t& dest)
                : m_curr_node(0), m_dest(dest) {

                using namespace nuwen;

                // The bitstrings 0, 1, 00, 01, 10, 11, 000, 001, ... are assigned the IDs 0, 1, 2, 3, 4, 5, 6, 7, ...
                // This corresponds to (1 << numbits) - 2 + masked_byte, where masked_byte is (byte & (1 << numbits) - 1).
                // Looking up id_table[numbits][byte] is faster than computing the ID.

                uc_t id_table[8][256];

                for (int numbits = 1; numbits <= 7; ++numbits) {
                    for (int byte = 0; byte < 256; ++byte) {
                        id_table[numbits][byte] = static_cast<uc_t>((1 << numbits) - 2 + (byte & ((1 << numbits) - 1)));
                    }
                }


                std::vector<node> nodes;

                const node * const root = regen_tree(nodes, codelengths, codes);

                // Build the auxiliary tables.

                uc_t aux_dest[254];

                for (int numbits = 1; numbits <= 7; ++numbits) {
                    for (uc_t masked_byte = 0; masked_byte < (1 << numbits); ++masked_byte) {
                        // Although we traverse the bitstrings in ID order, using id_table is simpler and isn't slower.
                        const uc_t id = id_table[numbits][masked_byte];

                        m_aux_numemit[id] = 0;

                        const node * p = root; // We have just emitted a symbol and have returned to the root.

                        // Process the bits of masked_byte from left to right.
                        for (int k = numbits - 1; k >= 0; --k) {
                            const bool bit = masked_byte >> k & 1;

                            p = bit ? p->right() : p->left();

                            if (p->left() == NULL) {
                                m_aux_table[id][m_aux_numemit[id]++] = p->byte();

                                p = root;
                            }
                        }

                        aux_dest[id] = static_cast<uc_t>(p - root);
                    }
                }


                // Build the main table.

                for (int possible_node = 0; possible_node < 255; ++possible_node) {
                    for (int possible_byte = 0; possible_byte < 256; ++possible_byte) {
                        entry * const ent = &m_main_table[possible_node][possible_byte];

                        ent->aux_id = ID_EMIT_NO_SYMBOLS;

                        const node * p = root + possible_node;

                        // Process the bits of possible_byte from left to right.
                        for (int k = 7; k >= 0; --k) {
                            const bool bit = possible_byte >> k & 1;

                            p = bit ? p->right() : p->left();

                            if (p->left() == NULL) {
                                ent->aux_id = ID_EMIT_ONE_SYMBOL;
                                ent->first_symbol = p->byte();

                                if (k == 0) {
                                    p = root;
                                } else {
                                    const uc_t id = id_table[k][possible_byte];

                                    p = root + aux_dest[id];

                                    if (m_aux_numemit[id] > 0) {
                                        ent->aux_id = id;
                                    }
                                }

                                break;
                            }
                        }

                        ent->dest_node = static_cast<uc_t>(p - root);
                    }
                }
            }

            void operator()(nuwen::vuc_ci_t byte, const nuwen::vuc_ci_t end) {
                for (; byte != end; ++byte) {
                    const entry * const p = &m_main_table[m_curr_node][*byte];
                    m_curr_node = p->dest_node;

                    switch (p->aux_id) {
                        case ID_EMIT_NO_SYMBOLS:
                            break;
                        case ID_EMIT_ONE_SYMBOL:
                            m_dest.push_back(p->first_symbol);
                            break;
                        default:
                            m_dest.push_back(p->first_symbol);

                            for (int i = 0; i < m_aux_numemit[p->aux_id]; ++i) {
                                m_dest.push_back(m_aux_table[p->aux_id][i]);
                            }

                            break;
                    }
                }
            }
        };

        inline nuwen::vuc_t puff_auto(const nuwen::vuc_t& v
            #ifdef PHAM_AUTOMATON_TIMING
                , double * const ctor_time = NULL
            #endif
        ) {
            const nuwen::vuc_t codelengths(v.begin(), v.begin() + 256);
            const nuwen::vuc_t codes = make_codes(codelengths);

            nuwen::vuc_t ret;

            ret.reserve(v.size() * 2); // A reasonable heuristic.

            #ifdef PHAM_AUTOMATON_TIMING
                const nuwen::chrono::watch w;
            #endif

            puff_automaton automaton(codelengths, codes, ret);

            #ifdef PHAM_AUTOMATON_TIMING
                if (ctor_time) {
                    *ctor_time = w.seconds();
                }
            #endif

            automaton(v.begin() + 256, v.end());

            return ret;
        }
    }
}

inline nuwen::vuc_t nuwen::huff(const vuc_t& v) {
    // huff_automaton construction is so fast, we may as well always use it.
    return pham::huff::huff_auto(v);
}

inline nuwen::vuc_t nuwen::puff(const vuc_t& v) {
    if (v.size() < 256) {
        throw std::runtime_error("RUNTIME ERROR: nuwen::puff() - Insufficient data to decompress.");
    }

    // For compressed data smaller than 6 KB, bitwise decompression is faster.
    return v.size() < 6144 ? pham::huff::puff_bits(v) : pham::huff::puff_auto(v);
}

#endif // Idempotency
