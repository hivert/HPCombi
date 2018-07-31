//****************************************************************************//
//       Copyright (C) 2016 Florent Hivert <Florent.Hivert@lri.fr>,           //
//                                                                            //
//  Distributed under the terms of the GNU General Public License (GPL)       //
//                                                                            //
//    This code is distributed in the hope that it will be useful,            //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of          //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
//   General Public License for more details.                                 //
//                                                                            //
//  The full text of the GPL is available at:                                 //
//                                                                            //
//                  http://www.gnu.org/licenses/                              //
//****************************************************************************//

// This is the implementation par of epu.hpp this should be seen as
// implementation details and should not be included directly.

#include <iostream>
#include <initializer_list>
#include <random>

// Comparison mode for _mm_cmpestri
#define FIRST_DIFF                                                             \
    (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY)
#define LAST_DIFF                                                              \
    (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY |        \
     _SIDD_MOST_SIGNIFICANT)
#define FIRST_ZERO (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY)
#define LAST_ZERO                                                              \
    (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_MOST_SIGNIFICANT)
#define FIRST_NON_ZERO                                                         \
    (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_MASKED_NEGATIVE_POLARITY)
#define LAST_NON_ZERO                                                          \
    (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_MASKED_NEGATIVE_POLARITY |  \
     _SIDD_MOST_SIGNIFICANT)

namespace HPCombi {

template <class TPU>
inline TPU TPUBuild<TPU>::operator()(std::initializer_list<type_elem> il,
                                     type_elem def) const {
    TPU res;
    assert(il.size() <= size);
    std::copy(il.begin(), il.end(), as_array(res).begin());
    for (size_t i = il.size(); i < size; ++i)
        res[i] = def;
    return res;
}


/*****************************************************************************/
/** Implementation part for inline functions *********************************/
/*****************************************************************************/

inline uint64_t first_diff_ref(epu8 a, epu8 b, size_t bound) {
    for (size_t i=0; i<bound; i++)
        if (a[i] != b[i]) return i;
    return 16;
}
inline uint64_t first_diff_cmpstr(epu8 a, epu8 b, size_t bound) {
    return unsigned(_mm_cmpestri(a, bound, b, bound, FIRST_DIFF));
}
inline uint64_t first_diff_mask(epu8 a, epu8 b, size_t bound) {
    auto res = _mm_movemask_epi8((a != b) && (epu8id < Epu8(bound)));
    return  res == 0 ? 16 : _bit_scan_forward(res);
}


inline uint64_t last_diff_ref(epu8 a, epu8 b, size_t bound) {
    while (bound != 0) {
        --bound;
        if (a[bound] != b[bound]) return bound;
    }
    return 16;
}
inline uint64_t last_diff_cmpstr(epu8 a, epu8 b, size_t bound) {
    return unsigned(_mm_cmpestri(a, bound, b, bound, LAST_DIFF));
}
inline uint64_t last_diff_mask(epu8 a, epu8 b, size_t bound) {
    auto res = _mm_movemask_epi8((a != b) && (epu8id < Epu8(bound)));
    return  res == 0 ? 16 : _bit_scan_reverse(res);
}

inline bool less(epu8 a, epu8 b) {
    uint64_t diff = first_diff(a, b);
    return (diff < 16) && a[diff] < b[diff];
}
inline char less_partial(epu8 a, epu8 b, int k) {
    uint64_t diff = first_diff(a, b, k);
    return (diff == 16)
               ? 0
               : static_cast<char>(a[diff]) - static_cast<char>(b[diff]);
}

/// Apply a sorting network
template <bool Increassing = true, size_t sz>
inline epu8 network_sort(epu8 res, std::array<epu8, sz> rounds) {
    for (auto round : rounds) {
        // This conditional should be optimized out by the compiler
        epu8 mask = Increassing ? round < epu8id : epu8id < round;
        epu8 b = permuted(res, round);
        // res = mask ? min(res,b) : max(res,b); is not accepted by clang
        res = _mm_blendv_epi8(min(res,b), max(res,b), mask);
    }
    return res;
}

/** A 16-way sorting network
 * @details Sorting network from Knuth [AoCP3] Fig. 51 p 229.
 * used by the #sorted function
 *
 * [AoCP3]: "D. Knuth, The art of computer programming vol. 3"
 */
constexpr std::array<epu8, 9> sorting_rounds
// clang-format off
    //     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
{{
    epu8 { 1,  0,  3,  2,  5,  4,  7,  6,  9,  8, 11, 10, 13, 12, 15, 14},
    epu8 { 2,  3,  0,  1,  6,  7,  4,  5, 10, 11,  8,  9, 14, 15, 12, 13},
    epu8 { 4,  5,  6,  7,  0,  1,  2,  3, 12, 13, 14, 15,  8,  9, 10, 11},
    epu8 { 8,  9, 10, 11, 12, 13, 14, 15,  0,  1,  2,  3,  4,  5,  6,  7},
    epu8 { 0,  2,  1, 12,  8, 10,  9, 11,  4,  6,  5,  7,  3, 14, 13, 15},
    epu8 { 0,  4,  8, 10,  1,  9, 12, 13,  2,  5,  3, 14,  6,  7, 11, 15},
    epu8 { 0,  1,  4,  5,  2,  3,  8,  9,  6,  7, 12, 13, 10, 11, 14, 15},
    epu8 { 0,  1,  2,  6,  4,  8,  3, 10,  5, 12,  7, 11,  9, 13, 14, 15},
    epu8 { 0,  1,  2,  4,  3,  6,  5,  8,  7, 10,  9, 12, 11, 13, 14, 15}
}};
// clang-format on

/** A duplicated 8-way sorting network
 * @details [Batcher odd-Even mergesort] sorting network
 * used by the #sorted function
 *
 * [Batcher odd-Even mergesort]: https://en.wikipedia.org/wiki/Batcher_odd%E2%80%93even_mergesort "Batcher odd–even mergesort"
 */
constexpr std::array<epu8, 6> sorting_rounds8
// clang-format off
    //     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
{{
    epu8 { 1,  0,  3,  2,  5,  4,  7,  6,  9,  8, 11, 10, 13, 12, 15, 14},
    epu8 { 2,  3,  0,  1,  6,  7,  4,  5, 10, 11,  8,  9, 14, 15, 12, 13},
    epu8 { 0,  2,  1,  3,  4,  6,  5,  7,  8, 10,  9, 11, 12, 14, 13, 15},
    epu8 { 4,  5,  6,  7,  0,  1,  2,  3, 12, 13, 14, 15,  8,  9, 10, 11},
    epu8 { 0,  1,  4,  5,  2,  3,  6,  7,  8,  9, 12, 13, 10, 11, 14, 15},
    epu8 { 0,  2,  1,  4,  3,  6,  5,  7,  8, 10,  9, 12, 11, 14, 13, 15}
}};
// clang-format on

inline bool is_sorted(epu8 a) {
    return _mm_movemask_epi8(shifted_right(a) > a) == 0;
}
inline epu8 sorted(epu8 a) {
    return network_sort<true>(a, sorting_rounds);
}
inline epu8 sorted8(epu8 a) {
    return network_sort<true>(a, sorting_rounds8);
};
inline epu8 revsorted(epu8 a) {
    return network_sort<false>(a, sorting_rounds);
}
inline epu8 revsorted8(epu8 a) {
    return network_sort<false>(a, sorting_rounds8);
};


inline epu8 random_epu8(uint16_t bnd) {
    epu8 res;
    std::random_device rd;

    std::default_random_engine e1(rd());
    std::uniform_int_distribution<int> uniform_dist(0, bnd - 1);
    for (size_t i = 0; i < 16; i++)
        res[i] = uniform_dist(e1);
    return res;
}

inline epu8 remove_dups(epu8 v, uint8_t repl) {
    // Ternary operator is not supported by clang.
    // return (v != permuted(Perm16::right_shift_ff()).v) ? v : cst_epu8_0x00;
    return _mm_blendv_epi8(Epu8(repl), v, v != shifted_right(v));
}


#if defined(FF)
#error FF is defined !
#endif /* FF */
#define FF 0xff

/// Permutation Round for partial and horizontal sums
constexpr std::array<epu8, 4> summing_rounds
    //      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
// clang-format off
{{
    epu8 { FF,  0, FF,  2, FF,  4, FF,  6, FF,  8, FF, 10, FF, 12, FF, 14},
    epu8 { FF, FF,  1,  1, FF, FF,  5,  5, FF, FF,  9,  9, FF, FF, 13, 13},
    epu8 { FF, FF, FF, FF,  3,  3,  3,  3, FF, FF, FF, FF, 11, 11, 11, 11},
    epu8 { FF, FF, FF, FF, FF, FF, FF, FF,  7,  7,  7,  7,  7,  7,  7,  7}
}};
// clang-format on

#undef FF

inline uint8_t horiz_sum_ref(epu8 v) {
    uint8_t res = 0;
    for (size_t i = 0; i < 16; i++) res += v[i];
    return res;
}

inline uint8_t horiz_sum4(epu8 v) {
    return partial_sums_round(v)[15];
}

inline uint8_t horiz_sum3(epu8 v) {
    auto sr = summing_rounds;
    v += permuted(v, sr[0]);
    v += permuted(v, sr[1]);
    v += permuted(v, sr[2]);
    return v[7] + v[15];
}

inline epu8 partial_sums_ref(epu8 v)  {
    epu8 res {};
    res[0] = v[0];
    for (size_t i = 1; i < 16; i++)
        res[i] = res[i - 1] + v[i];
    return res;
}
inline epu8 partial_sums_round(epu8 v) {
    for (epu8 round : summing_rounds)
        v += permuted(v, round);
    return v;
}


inline epu8 eval16_ref(epu8 v) {
    epu8 res {};
    for (size_t i = 0; i < 16; i++)
        if (v[i] < 16)
            res[v[i]]++;
    return res;
}

inline epu8 eval16_arr(epu8 v8) {
    TPUBuild<epu8>::array res {};
    auto v = as_array(v8);
    for (size_t i = 0; i < 16; i++)
        if (v[i] < 16)
            res[v[i]]++;
    return from_array(res);
}

inline epu8 eval16_cycle(epu8 v) {
    epu8 res = -(epu8id == v);
    for (int i = 1; i < 16; i++) {
        v = permuted(v, left_cycle);
        res -= (epu8id == v);
    }
    return res;
}

inline epu8 eval16_popcount(epu8 v) {
    epu8 res {};
    for (size_t i = 0; i < 16; i++) {
        res[i] = _mm_popcnt_u32(_mm_movemask_epi8(v == Epu8(uint8_t(i))));
    }
    return res;
}

}  // namespace HPCombi

namespace std {

inline std::ostream &operator<<(std::ostream &stream, HPCombi::epu8 const &a) {
    stream << "[" << std::setw(2) << unsigned(a[0]);
    for (unsigned i = 1; i < 16; ++i)
        stream << "," << std::setw(2) << unsigned(a[i]);
    stream << "]";
    return stream;
}

template <> struct equal_to<HPCombi::epu8> {
    bool operator()(const HPCombi::epu8 &lhs, const HPCombi::epu8 &rhs) const {
        return HPCombi::equal(lhs, rhs);
    }
};

template <> struct not_equal_to<HPCombi::epu8> {
    bool operator()(const HPCombi::epu8 &lhs, const HPCombi::epu8 &rhs) const {
        return HPCombi::not_equal(lhs, rhs);
    }
};

template <> struct hash<HPCombi::epu8> {
    inline size_t operator()(HPCombi::epu8 a) const {
        __int128 v0 = _mm_extract_epi64(a, 0);
        __int128 v1 = _mm_extract_epi64(a, 1);
        return ((v1 * HPCombi::prime + v0) * HPCombi::prime) >> 64;

        /* The following is extremely slow on Renner benchmark
           uint64_t v0 = _mm_extract_epi64(ar.v, 0);
           uint64_t v1 = _mm_extract_epi64(ar.v, 1);
           size_t seed = v0 + 0x9e3779b9;
           seed ^= v1 + 0x9e3779b9 + (seed<<6) + (seed>>2);
           return seed;
        */
    }
};


template <> struct less<HPCombi::epu8> {
  // WARNING: due to endianess this is not lexicographic comparison,
  //          but we don't care when using in std::set.
  // 10% faster than calling the lexicographic comparison operator !
    inline size_t operator()(const HPCombi::epu8 &v1,
                             const HPCombi::epu8 &v2) const {
        __m128 v1v = __m128(v1), v2v = __m128(v2);
        return v1v[0] == v2v[0] ? v1v[1] < v2v[1] : v1v[0] < v2v[0];
    }
};

}  // namespace std

