/******************************************************************************/
/*     Copyright (C) 2016-2018 Florent Hivert <Florent.Hivert@lri.fr>,        */
/*                                                                            */
/*  Distributed under the terms of the GNU General Public License (GPL)       */
/*                                                                            */
/*    This code is distributed in the hope that it will be useful,            */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       */
/*   General Public License for more details.                                 */
/*                                                                            */
/*  The full text of the GPL is available at:                                 */
/*                                                                            */
/*                  http://www.gnu.org/licenses/                              */
/******************************************************************************/

#define BOOST_TEST_MODULE EPUTests

#include <boost/functional.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>

#include "epu.hpp"
#include <iostream>

using namespace HPCombi;

#define EPU8_EQUAL(p1, p2) BOOST_CHECK_PREDICATE(equal, (p1)(p2))
#define EPU8_NOT_EQUAL(p1, p2)                                                 \
    BOOST_CHECK_PREDICATE(boost::not2(equal), (p1)(p2))

#define TEST_AGREES(ref, fun)                                                  \
    BOOST_FIXTURE_TEST_CASE(EPU8_agrees_##fun, Fix) {                          \
        for (auto x : v)                                                       \
            BOOST_TEST(fun(x) == ref(x));                                      \
    }
#define TEST_EPU8_AGREES(ref, fun)                                             \
    BOOST_FIXTURE_TEST_CASE(EPU8_agrees_##fun, Fix) {                          \
        for (auto x : v)                                                       \
            EPU8_EQUAL(fun(x), ref(x));                                        \
    }

struct Fix {
    Fix()
        : zero(Epu8({}, 0)), P01(Epu8({0, 1}, 0)), P10(Epu8({1, 0}, 0)),
          P11(Epu8({1, 1}, 0)), P1(Epu8({}, 1)), P112(Epu8({1, 1}, 2)),
          Pa(epu8{1, 2, 3, 4, 0, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
          Pb(epu8{1, 2, 3, 6, 0, 5, 4, 7, 8, 9, 10, 11, 12, 15, 14, 13}),
          RP(epu8{3, 1, 0, 14, 15, 13, 5, 10, 2, 11, 6, 12, 7, 4, 8, 9}),
          Pa1(Epu8({4, 2, 5, 1, 2, 7, 7, 3, 4, 2}, 1)),
          Pa2(Epu8({4, 2, 5, 1, 2, 9, 7, 3, 4, 2}, 1)), P51(Epu8({5, 1}, 6)),
          Pv(epu8{5, 5, 2, 5, 1, 6, 12, 4, 0, 3, 2, 11, 12, 13, 14, 15}),
          Pw(epu8{5, 5, 2, 9, 1, 6, 12, 4, 0, 4, 4, 4, 12, 13, 14, 15}),
          P5(Epu8({}, 5)), Pc(Epu8({23, 5, 21, 5, 43, 36}, 7)),
          // Elements should be sorted in alphabetic order here
          v({zero, P01, epu8id, P10, P11, P1, P112, Pa, Pb, RP, Pa1, Pa2, P51,
             Pv, Pw, P5, epu8rev, Pc}),
          av({{5, 5, 2, 5, 1, 6, 12, 4, 0, 3, 2, 11, 12, 13, 14, 15}}) {
        BOOST_TEST_MESSAGE("setup fixture");
    }
    ~Fix() { BOOST_TEST_MESSAGE("teardown fixture"); }

    const epu8 zero, P01, P10, P11, P1, P112, Pa, Pb, RP, Pa1, Pa2, P51, Pv, Pw,
        P5, Pc;
    const std::vector<epu8> v;
    const std::array<uint8_t, 16> av;
};

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_compare)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(EPU8_first_diff_ref, Fix) {
    BOOST_TEST(first_diff_ref(Pc, Pc) == 16);
    BOOST_TEST(first_diff_ref(zero, P01) == 1);
    BOOST_TEST(first_diff_ref(zero, P10) == 0);
    BOOST_TEST(first_diff_ref(zero, P01, 1) == 16);
    BOOST_TEST(first_diff_ref(zero, P01, 2) == 1);
    BOOST_TEST(first_diff_ref(Pa1, Pa2, 2) == 16);
    BOOST_TEST(first_diff_ref(Pa1, Pa2, 4) == 16);
    BOOST_TEST(first_diff_ref(Pa1, Pa2, 5) == 16);
    BOOST_TEST(first_diff_ref(Pa1, Pa2, 6) == 5);
    BOOST_TEST(first_diff_ref(Pa1, Pa2, 7) == 5);
    BOOST_TEST(first_diff_ref(Pa1, Pa2) == 5);
    BOOST_TEST(first_diff(Pv, Pw) == 3);
    for (int i = 0; i < 16; i++)
        BOOST_TEST(first_diff(Pv, Pw, i) == (i <= 3 ? 16 : 3));
}
BOOST_FIXTURE_TEST_CASE(EPU8_first_diff_cmpstr, Fix) {
    for (auto x : v) {
        for (auto y : v) {
            BOOST_TEST(first_diff_cmpstr(x, y) == first_diff_ref(x, y));
            for (int i = 0; i < 17; i++)
                BOOST_TEST(first_diff_cmpstr(x, y, i) ==
                           first_diff_ref(x, y, i));
        }
    }
}
BOOST_FIXTURE_TEST_CASE(EPU8_first_diff_mask, Fix) {
    for (auto x : v) {
        for (auto y : v) {
            BOOST_TEST(first_diff_mask(x, y) == first_diff_ref(x, y));
            for (int i = 0; i < 17; i++)
                BOOST_TEST(first_diff_mask(x, y, i) == first_diff_ref(x, y, i));
        }
    }
}

BOOST_FIXTURE_TEST_CASE(EPU8_last_diff_ref, Fix) {
    BOOST_TEST(last_diff_ref(Pc, Pc) == 16);
    BOOST_TEST(last_diff_ref(zero, P01) == 1);
    BOOST_TEST(last_diff_ref(zero, P10) == 0);
    BOOST_TEST(last_diff_ref(zero, P01, 1) == 16);
    BOOST_TEST(last_diff_ref(zero, P01, 2) == 1);
    BOOST_TEST(last_diff_ref(P1, Pa1) == 9);
    BOOST_TEST(last_diff_ref(P1, Pa1, 12) == 9);
    BOOST_TEST(last_diff_ref(P1, Pa1, 9) == 8);
    BOOST_TEST(last_diff_ref(Pa1, Pa2, 2) == 16);
    BOOST_TEST(last_diff_ref(Pa1, Pa2, 4) == 16);
    BOOST_TEST(last_diff_ref(Pa1, Pa2, 5) == 16);
    BOOST_TEST(last_diff_ref(Pa1, Pa2, 6) == 5);
    BOOST_TEST(last_diff_ref(Pa1, Pa2, 7) == 5);
    BOOST_TEST(last_diff_ref(Pa1, Pa2) == 5);
    const std::array<uint8_t, 17> res{
        {16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 9, 10, 11, 11, 11, 11, 11}};
    for (int i = 0; i <= 16; i++)
        BOOST_TEST(last_diff_ref(Pv, Pw, i) == res[i]);
}
BOOST_FIXTURE_TEST_CASE(EPU8_last_diff_cmpstr, Fix) {
    for (auto x : v) {
        for (auto y : v) {
            BOOST_TEST(last_diff_cmpstr(x, y) == last_diff_ref(x, y));
            for (int i = 0; i < 17; i++)
                BOOST_TEST(last_diff_cmpstr(x, y, i) == last_diff_ref(x, y, i));
        }
    }
}
BOOST_FIXTURE_TEST_CASE(EPU8_last_diff_mask, Fix) {
    for (auto x : v) {
        for (auto y : v) {
            BOOST_TEST(last_diff_mask(x, y) == last_diff_ref(x, y));
            for (int i = 0; i < 17; i++)
                BOOST_TEST(last_diff_mask(x, y, i) == last_diff_ref(x, y, i));
        }
    }
}

BOOST_FIXTURE_TEST_CASE(EPU8_is_all_zero, Fix) {
    BOOST_TEST(is_all_zero(zero));
    for (size_t i = 1; i < v.size(); i++) {
        BOOST_TEST(not is_all_zero(v[i]));
    }
}

BOOST_FIXTURE_TEST_CASE(EPU8_is_all_one, Fix) {
    for (size_t i = 0; i < v.size(); i++) {
        BOOST_TEST(not is_all_one(v[i]));
    }
    BOOST_TEST(is_all_one(Epu8(0xFF)));
}

BOOST_FIXTURE_TEST_CASE(EPU8_equal, Fix) {
    for (size_t i = 0; i < v.size(); i++) {
        epu8 a = v[i];
        for (size_t j = 0; j < v.size(); j++) {
            epu8 b = v[j];
            if (i == j) {
                BOOST_CHECK_PREDICATE(equal, (a)(b));
                BOOST_CHECK_PREDICATE(boost::not2(not_equal), (a)(b));
                BOOST_CHECK_PREDICATE(std::equal_to<epu8>(), (a)(b));
                //  For some reason, the following line doesn't compile
                //              BOOST_CHECK_PREDICATE(boost::not2(std::not_equal_to<epu8>()),
                //                                    (a)(b));
                BOOST_CHECK_PREDICATE(
                    [](epu8 a, epu8 b) {
                        return not std::not_equal_to<epu8>()(a, b);
                    },
                    (a)(b));
            } else {
                BOOST_CHECK_PREDICATE(boost::not2(equal), (a)(b));
                BOOST_CHECK_PREDICATE(not_equal, (a)(b));
                BOOST_CHECK_PREDICATE(std::not_equal_to<epu8>(), (a)(b));
                //  For some reason, the following line doesn't compile
                //              BOOST_CHECK_PREDICATE(boost::not2(std::equal_to<epu8>()),
                //              (a)(b));
                BOOST_CHECK_PREDICATE(
                    [](epu8 a, epu8 b) {
                        return not std::equal_to<epu8>()(a, b);
                    },
                    (a)(b));
            }
        }
    }
}

BOOST_FIXTURE_TEST_CASE(EPU8_not_equal, Fix) {
    for (size_t i = 0; i < v.size(); i++)
        for (size_t j = 0; j < v.size(); j++)
            if (i == j)
                BOOST_CHECK_PREDICATE(boost::not2(not_equal), (v[i])(v[j]));
            else
                BOOST_CHECK_PREDICATE(not_equal, (v[i])(v[j]));
}

BOOST_FIXTURE_TEST_CASE(EPU8_less, Fix) {
    for (size_t i = 0; i < v.size(); i++)
        for (size_t j = 0; j < v.size(); j++)
            if (i < j)
                BOOST_CHECK_PREDICATE(less, (v[i])(v[j]));
            else
                BOOST_CHECK_PREDICATE(boost::not2(less), (v[i])(v[j]));
}
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_permute)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(EPU8_permuted, Fix) {
    EPU8_EQUAL(
        permuted(epu8{0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                 epu8{3, 2, 5, 1, 4, 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
        (epu8{2, 3, 5, 1, 4, 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}));
    EPU8_EQUAL(
        permuted(epu8{3, 2, 5, 1, 4, 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                 epu8{0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
        (epu8{3, 2, 1, 5, 4, 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}));
    EPU8_EQUAL(
        permuted(epu8{3, 2, 5, 1, 4, 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                 epu8{2, 2, 1, 2, 3, 6, 12, 4, 5, 16, 17, 11, 12, 13, 14, 15}),
        (epu8{5, 5, 2, 5, 1, 6, 12, 4, 0, 3, 2, 11, 12, 13, 14, 15}));
}

BOOST_FIXTURE_TEST_CASE(EPU8_shifted_left, Fix) {
    EPU8_EQUAL(shifted_left(P01), P10);
    EPU8_EQUAL(shifted_left(P112),
               (epu8{1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0}));
    EPU8_EQUAL(shifted_left(Pv),
               (epu8{5, 2, 5, 1, 6, 12, 4, 0, 3, 2, 11, 12, 13, 14, 15, 0}));
}

BOOST_FIXTURE_TEST_CASE(EPU8_shifted_right, Fix) {
    EPU8_EQUAL(shifted_right(P10), P01);
    EPU8_EQUAL(shifted_right(P112), Epu8({0, 1, 1}, 2));
    EPU8_EQUAL(shifted_right(Pv),
               (epu8{0, 5, 5, 2, 5, 1, 6, 12, 4, 0, 3, 2, 11, 12, 13, 14}));
}

BOOST_FIXTURE_TEST_CASE(EPU8_reverted, Fix) {
    EPU8_EQUAL(reverted(epu8id), epu8rev);
    for (auto x : v)
        EPU8_EQUAL(x, reverted(reverted(x)));
}
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_array)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(EPU8_as_array, Fix) {
    epu8 x = Epu8({4, 2, 5, 1, 2, 7, 7, 3, 4, 2}, 1);
    auto &refx = as_array(x);
    refx[2] = 42;
    EPU8_EQUAL(x, Epu8({4, 2, 42, 1, 2, 7, 7, 3, 4, 2}, 1));
    std::fill(refx.begin() + 4, refx.end(), 3);
    EPU8_EQUAL(x, Epu8({4, 2, 42, 1}, 3));
    BOOST_TEST(av == as_array(Pv));
}

BOOST_FIXTURE_TEST_CASE(EPU8_from_array, Fix) {
    for (auto x : v) {
        EPU8_EQUAL(x, from_array(as_array(x)));
    }
    EPU8_EQUAL(Pv, from_array(av));
}
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_sorting)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(EPU8_is_sorted, Fix) {
    BOOST_TEST(is_sorted(epu8id));
    BOOST_TEST(
        is_sorted(epu8{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}));
    BOOST_TEST(is_sorted(Epu8({0, 1}, 2)));
    BOOST_TEST(is_sorted(Epu8({0}, 1)));
    BOOST_TEST(is_sorted(Epu8({}, 5)));
    BOOST_TEST(not is_sorted(
        epu8{0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}));
    BOOST_TEST(not is_sorted(Epu8({0, 2}, 1)));
    BOOST_TEST(not is_sorted(Epu8({0, 0, 2}, 1)));
    BOOST_TEST(not is_sorted(Epu8({6}, 5)));

    epu8 x = epu8id;
    BOOST_TEST(is_sorted(x));
    auto &refx = as_array(x);
    while (std::next_permutation(refx.begin(), refx.begin() + 9)) {
        BOOST_TEST(not is_sorted(x));
    }
    x = epu8id;
    while (std::next_permutation(refx.begin() + 8, refx.begin() + 16)) {
        BOOST_TEST(not is_sorted(x));
    }
    x = sorted(Pa1);
    BOOST_TEST(is_sorted(x));
    while (std::next_permutation(refx.begin(), refx.begin() + 14)) {
        BOOST_TEST(not is_sorted(x));
    }
}

BOOST_FIXTURE_TEST_CASE(EPU8_sorted, Fix) {
    EPU8_EQUAL(
        sorted(epu8{0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
        epu8id);
    for (auto &x : v)
        BOOST_TEST(is_sorted(sorted(x)));
    epu8 x = epu8id;
    BOOST_TEST(is_sorted(x));
    auto &refx = as_array(x);
    do {
        BOOST_TEST(is_sorted(sorted(x)));
    } while (std::next_permutation(refx.begin(), refx.begin() + 9));
}

BOOST_FIXTURE_TEST_CASE(EPU8_revsorted, Fix) {
    EPU8_EQUAL(
        revsorted(epu8{0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
        epu8rev);
    for (auto &x : v)
        BOOST_TEST(is_sorted(reverted(revsorted(x))));
    epu8 x = epu8id;
    BOOST_TEST(is_sorted(x));
    auto &refx = as_array(x);
    do {
        BOOST_TEST(is_sorted(reverted(revsorted(x))));
    } while (std::next_permutation(refx.begin(), refx.begin() + 9));
}

BOOST_FIXTURE_TEST_CASE(EPU8_sort_perm, Fix) {
    epu8 ve{2, 1, 3, 2, 4, 1, 1, 4, 2, 0, 1, 2, 1, 3, 4, 0};
    EPU8_EQUAL(sort_perm(ve),
               (epu8{9, 15, 1, 5, 6, 10, 12, 3, 0, 8, 11, 2, 13, 7, 4, 14}));
    EPU8_EQUAL(ve, (epu8{0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4, 4, 4}));

    for (auto x : v) {
        epu8 xsort = x;
        epu8 psort = sort_perm(xsort);
        BOOST_TEST(is_sorted(xsort));
        BOOST_TEST(is_permutation(psort));
        EPU8_EQUAL(permuted(x, psort), xsort);
    }
}

BOOST_FIXTURE_TEST_CASE(EPU8_sort8_perm, Fix) {
    epu8 ve{2, 1, 3, 2, 4, 1, 1, 4, 2, 0, 1, 2, 1, 3, 4, 0};
    EPU8_EQUAL(sort8_perm(ve),
               (epu8{1, 6, 5, 0, 3, 2, 4, 7, 9, 15, 10, 12, 8, 11, 13, 14}));
    EPU8_EQUAL(ve, (epu8{1, 1, 1, 2, 2, 3, 4, 4, 0, 0, 1, 1, 2, 2, 3, 4}));

    for (auto x : v) {
        epu8 xsort = x;
        epu8 psort = sort_perm(xsort);
        BOOST_TEST(is_sorted(xsort | Epu8({0, 0, 0, 0, 0, 0, 0, 0}, 0xFF)));
        BOOST_TEST(is_sorted(xsort & Epu8({0, 0, 0, 0, 0, 0, 0, 0}, 0xFF)));
        BOOST_TEST(is_permutation(psort));
        EPU8_EQUAL(permuted(x, psort), xsort);
    }
}

BOOST_FIXTURE_TEST_CASE(EPU8_permutation_of, Fix) {
    EPU8_EQUAL(permutation_of(epu8id, epu8id), epu8id);
    EPU8_EQUAL(permutation_of(Pa, Pa), epu8id);
    EPU8_EQUAL(permutation_of(epu8rev, epu8id), epu8rev);
    EPU8_EQUAL(permutation_of(epu8id, epu8rev), epu8rev);
    EPU8_EQUAL(permutation_of(epu8rev, epu8rev), epu8id);
    EPU8_EQUAL(permutation_of(epu8id, RP), RP);
    const uint8_t FF = 0xff;
    EPU8_EQUAL(
        (permutation_of(Pv, Pv) |
         //                     0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15
         //               epu8{ 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15}
         (epu8{FF, FF, FF, FF, 0, 0, FF, 0, 0, 0, FF, 0, FF, 0, 0, 0})),
        (epu8{FF, FF, FF, FF, 4, 5, FF, 7, 8, 9, FF, 11, FF, 13, 14, 15}));
}
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_remove_dups_sum)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(EPU8_remove_dups, Fix) {
    EPU8_EQUAL(remove_dups(P1), P10);
    EPU8_EQUAL(remove_dups(P11), P10);
    EPU8_EQUAL(remove_dups(sorted(P10)),
               (epu8{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EPU8_EQUAL(remove_dups(sorted(Pv)),
               (epu8{0, 1, 2, 0, 3, 4, 5, 0, 0, 6, 11, 12, 0, 13, 14, 15}));
    EPU8_EQUAL(remove_dups(P1, 1), P1);
    EPU8_EQUAL(remove_dups(P11, 1), Epu8({1, 1, 0}, 1));
    EPU8_EQUAL(remove_dups(P11, 42), Epu8({1, 42, 0}, 42));
    EPU8_EQUAL(remove_dups(sorted(P10), 1), P1);
    EPU8_EQUAL(remove_dups(sorted(Pv), 7),
               (epu8{7, 1, 2, 7, 3, 4, 5, 7, 7, 6, 11, 12, 7, 13, 14, 15}));
    for (auto x : v) {
        x = sorted(remove_dups(sorted(x)));
        EPU8_EQUAL(x, sorted(remove_dups(x)));
    }
    for (auto x : v) {
        x = sorted(remove_dups(sorted(x), 42));
        EPU8_EQUAL(x, sorted(remove_dups(x, 42)));
    }
}
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_horiz_sum)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(EPU8_horiz_sum_ref, Fix) {
    BOOST_TEST(horiz_sum_ref(zero) == 0);
    BOOST_TEST(horiz_sum_ref(P01) == 1);
    BOOST_TEST(horiz_sum_ref(epu8id) == 120);
    BOOST_TEST(horiz_sum_ref(P10) == 1);
    BOOST_TEST(horiz_sum_ref(P11) == 2);
    BOOST_TEST(horiz_sum_ref(P1) == 16);
    BOOST_TEST(horiz_sum_ref(P112) == 30);
    BOOST_TEST(horiz_sum_ref(Pa1) == 43);
    BOOST_TEST(horiz_sum_ref(Pa2) == 45);
    BOOST_TEST(horiz_sum_ref(P51) == 90);
    BOOST_TEST(horiz_sum_ref(Pv) == 110);
    BOOST_TEST(horiz_sum_ref(P5) == 80);
    BOOST_TEST(horiz_sum_ref(epu8rev) == 120);
    BOOST_TEST(horiz_sum_ref(Pc) == 203);
}
TEST_AGREES(horiz_sum_ref, horiz_sum_gen)
TEST_AGREES(horiz_sum_ref, horiz_sum4)
TEST_AGREES(horiz_sum_ref, horiz_sum3)
TEST_AGREES(horiz_sum_ref, horiz_sum)
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_partial_sums)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(EPU8_partial_sums_ref, Fix) {
    EPU8_EQUAL(partial_sums_ref(zero), zero);
    EPU8_EQUAL(partial_sums_ref(P01), Epu8({0}, 1));
    EPU8_EQUAL(partial_sums_ref(epu8id), (epu8{0, 1, 3, 6, 10, 15, 21, 28, 36,
                                               45, 55, 66, 78, 91, 105, 120}));
    EPU8_EQUAL(partial_sums_ref(P10), P1);
    EPU8_EQUAL(partial_sums_ref(P11), Epu8({1}, 2));
    EPU8_EQUAL(partial_sums_ref(P1), epu8id + Epu8({}, 1));
    EPU8_EQUAL(partial_sums_ref(P112), (epu8{1, 2, 4, 6, 8, 10, 12, 14, 16, 18,
                                             20, 22, 24, 26, 28, 30}));
    EPU8_EQUAL(partial_sums_ref(Pa1), (epu8{4, 6, 11, 12, 14, 21, 28, 31, 35,
                                            37, 38, 39, 40, 41, 42, 43}));
    EPU8_EQUAL(partial_sums_ref(Pa2), (epu8{4, 6, 11, 12, 14, 23, 30, 33, 37,
                                            39, 40, 41, 42, 43, 44, 45}));
    EPU8_EQUAL(partial_sums_ref(P51), (epu8{5, 6, 12, 18, 24, 30, 36, 42, 48,
                                            54, 60, 66, 72, 78, 84, 90}));
    EPU8_EQUAL(partial_sums_ref(Pv), (epu8{5, 10, 12, 17, 18, 24, 36, 40, 40,
                                           43, 45, 56, 68, 81, 95, 110}));
    EPU8_EQUAL(partial_sums_ref(P5), (epu8{5, 10, 15, 20, 25, 30, 35, 40, 45,
                                           50, 55, 60, 65, 70, 75, 80}));
    EPU8_EQUAL(partial_sums_ref(epu8rev),
               (epu8{15, 29, 42, 54, 65, 75, 84, 92, 99, 105, 110, 114, 117,
                     119, 120, 120}));
    EPU8_EQUAL(partial_sums_ref(Pc),
               (epu8{23, 28, 49, 54, 97, 133, 140, 147, 154, 161, 168, 175, 182,
                     189, 196, 203}));
}
BOOST_FIXTURE_TEST_CASE(EPU8_partial_sum_gen, Fix) {
    for (auto x : v)
        EPU8_EQUAL(partial_sums_gen(x), partial_sums_ref(x));
}
BOOST_FIXTURE_TEST_CASE(EPU8_partial_sum_round, Fix) {
    for (auto x : v)
        EPU8_EQUAL(partial_sums_round(x), partial_sums_ref(x));
}
BOOST_FIXTURE_TEST_CASE(EPU8_partial_sum, Fix) {
    for (auto x : v)
        EPU8_EQUAL(partial_sums(x), partial_sums_ref(x));
}
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_horiz_max)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(EPU8_horiz_max_ref, Fix) {
    BOOST_TEST(horiz_max_ref(zero) == 0);
    BOOST_TEST(horiz_max_ref(P01) == 1);
    BOOST_TEST(horiz_max_ref(epu8id) == 15);
    BOOST_TEST(horiz_max_ref(P10) == 1);
    BOOST_TEST(horiz_max_ref(P11) == 1);
    BOOST_TEST(horiz_max_ref(P1) == 1);
    BOOST_TEST(horiz_max_ref(P112) == 2);
    BOOST_TEST(horiz_max_ref(Pa1) == 7);
    BOOST_TEST(horiz_max_ref(Pa2) == 9);
    BOOST_TEST(horiz_max_ref(P51) == 6);
    BOOST_TEST(horiz_max_ref(Pv) == 15);
    BOOST_TEST(horiz_max_ref(P5) == 5);
    BOOST_TEST(horiz_max_ref(epu8rev) == 15);
    BOOST_TEST(horiz_max_ref(Pc) == 43);
}
TEST_AGREES(horiz_max_ref, horiz_max_gen)
TEST_AGREES(horiz_max_ref, horiz_max4)
TEST_AGREES(horiz_max_ref, horiz_max3)
TEST_AGREES(horiz_max_ref, horiz_max)
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_partial_max)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(EPU8_partial_max_ref, Fix) {
    EPU8_EQUAL(partial_max_ref(zero), zero);
    EPU8_EQUAL(partial_max_ref(P01), Epu8({0}, 1));
    EPU8_EQUAL(partial_max_ref(epu8id), epu8id);
    EPU8_EQUAL(partial_max_ref(P10), P1);
    EPU8_EQUAL(partial_max_ref(P11), P1);
    EPU8_EQUAL(partial_max_ref(P1), P1);
    EPU8_EQUAL(partial_max_ref(P112), P112);
    EPU8_EQUAL(partial_max_ref(Pa1), (Epu8({4, 4, 5, 5, 5}, 7)));
    EPU8_EQUAL(partial_max_ref(Pa2), (Epu8({4, 4, 5, 5, 5}, 9)));
    EPU8_EQUAL(partial_max_ref(P51), (Epu8({5, 5}, 6)));
    EPU8_EQUAL(partial_max_ref(Pv), (epu8{5, 5, 5, 5, 5, 6, 12, 12, 12, 12, 12,
                                          12, 12, 13, 14, 15}));
    EPU8_EQUAL(partial_max_ref(P5), P5);
    EPU8_EQUAL(partial_max_ref(epu8rev), Epu8({}, 15));
    EPU8_EQUAL(partial_max_ref(Pc), (Epu8({23, 23, 23, 23}, 43)));
}
BOOST_FIXTURE_TEST_CASE(EPU8_partial_sum_gen, Fix) {
    for (auto x : v)
        EPU8_EQUAL(partial_max_gen(x), partial_max_ref(x));
}
BOOST_FIXTURE_TEST_CASE(EPU8_partial_max_round, Fix) {
    for (auto x : v)
        EPU8_EQUAL(partial_max_round(x), partial_max_ref(x));
}
BOOST_FIXTURE_TEST_CASE(EPU8_partial_max, Fix) {
    for (auto x : v)
        EPU8_EQUAL(partial_max(x), partial_max_ref(x));
}
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_horiz_min)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(EPU8_horiz_min_ref, Fix) {
    BOOST_TEST(horiz_min_ref(zero) == 0);
    BOOST_TEST(horiz_min_ref(P01) == 0);
    BOOST_TEST(horiz_min_ref(epu8id) == 0);
    BOOST_TEST(horiz_min_ref(P10) == 0);
    BOOST_TEST(horiz_min_ref(P11) == 0);
    BOOST_TEST(horiz_min_ref(P1) == 1);
    BOOST_TEST(horiz_min_ref(P112) == 1);
    BOOST_TEST(horiz_min_ref(Pa1) == 1);
    BOOST_TEST(horiz_min_ref(Pa2) == 1);
    BOOST_TEST(horiz_min_ref(P51) == 1);
    BOOST_TEST(horiz_min_ref(Pv) == 0);
    BOOST_TEST(horiz_min_ref(P5) == 5);
    BOOST_TEST(horiz_min_ref(epu8rev) == 0);
    BOOST_TEST(horiz_min_ref(Pc) == 5);
}
TEST_AGREES(horiz_min_ref, horiz_min_gen)
TEST_AGREES(horiz_min_ref, horiz_min4)
TEST_AGREES(horiz_min_ref, horiz_min3)
TEST_AGREES(horiz_min_ref, horiz_min)
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_partial_min)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(EPU8_partial_min_ref, Fix) {
    EPU8_EQUAL(partial_min_ref(zero), zero);
    EPU8_EQUAL(partial_min_ref(P01), zero);
    EPU8_EQUAL(partial_min_ref(epu8id), zero);
    EPU8_EQUAL(partial_min_ref(P10), P10);
    EPU8_EQUAL(partial_min_ref(P11), P11);
    EPU8_EQUAL(partial_min_ref(P1), P1);
    EPU8_EQUAL(partial_min_ref(P112), P1);
    EPU8_EQUAL(partial_min_ref(Pa1), (Epu8({4, 2, 2}, 1)));
    EPU8_EQUAL(partial_min_ref(Pa2), (Epu8({4, 2, 2}, 1)));
    EPU8_EQUAL(partial_min_ref(P51), (Epu8({5}, 1)));
    EPU8_EQUAL(partial_min_ref(Pv), (Epu8(
                                        {
                                            5,
                                            5,
                                            2,
                                            2,
                                            1,
                                            1,
                                            1,
                                            1,
                                        },
                                        0)));
    EPU8_EQUAL(partial_min_ref(P5), P5);
    EPU8_EQUAL(partial_min_ref(epu8rev), epu8rev);
    EPU8_EQUAL(partial_min_ref(Pc), (Epu8({23}, 5)));
}
BOOST_FIXTURE_TEST_CASE(EPU8_partial_sum_gen, Fix) {
    for (auto x : v)
        EPU8_EQUAL(partial_min_gen(x), partial_min_ref(x));
}
BOOST_FIXTURE_TEST_CASE(EPU8_partial_min_round, Fix) {
    for (auto x : v)
        EPU8_EQUAL(partial_min_round(x), partial_min_ref(x));
}
BOOST_FIXTURE_TEST_CASE(EPU8_partial_min, Fix) {
    for (auto x : v)
        EPU8_EQUAL(partial_min(x), partial_min_ref(x));
}
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_eval16)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(EPU8_eval16_ref, Fix) {
    EPU8_EQUAL(eval16_ref(zero), Epu8({16}, 0));
    EPU8_EQUAL(eval16_ref(P01), Epu8({15, 1}, 0));
    EPU8_EQUAL(eval16_ref(epu8id), Epu8({}, 1));
    EPU8_EQUAL(eval16_ref(P10), Epu8({15, 1}, 0));
    EPU8_EQUAL(eval16_ref(P11), Epu8({14, 2}, 0));
    EPU8_EQUAL(eval16_ref(P1), Epu8({0, 16}, 0));
    EPU8_EQUAL(eval16_ref(P112), Epu8({0, 2, 14}, 0));
    EPU8_EQUAL(eval16_ref(Pa1), Epu8({0, 7, 3, 1, 2, 1, 0, 2}, 0));
    EPU8_EQUAL(eval16_ref(Pa2), Epu8({0, 7, 3, 1, 2, 1, 0, 1, 0, 1}, 0));
    EPU8_EQUAL(eval16_ref(P51), Epu8({0, 1, 0, 0, 0, 1, 14}, 0));
    EPU8_EQUAL(eval16_ref(Pv),
               (epu8{1, 1, 2, 1, 1, 3, 1, 0, 0, 0, 0, 1, 2, 1, 1, 1}));
    EPU8_EQUAL(eval16_ref(P5), Epu8({0, 0, 0, 0, 0, 16}, 0));
    EPU8_EQUAL(eval16_ref(epu8rev), Epu8({}, 1));
    EPU8_EQUAL(eval16_ref(Pc), Epu8({0, 0, 0, 0, 0, 2, 0, 10}, 0));
}
TEST_EPU8_AGREES(eval16_ref, eval16_cycle)
TEST_EPU8_AGREES(eval16_ref, eval16_popcount)
TEST_EPU8_AGREES(eval16_ref, eval16_arr)
TEST_EPU8_AGREES(eval16_ref, eval16_gen)
TEST_EPU8_AGREES(eval16_ref, eval16)
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_vector_popcount)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(EPU8_popcount4, Fix) {
    EPU8_EQUAL(popcount4,
               (epu8{0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4}));
}

BOOST_FIXTURE_TEST_CASE(EPU8_popcount, Fix) {
    EPU8_EQUAL(popcount16(Pv),
               (epu8{2, 2, 1, 2, 1, 2, 2, 1, 0, 2, 1, 3, 2, 3, 3, 4}));
    EPU8_EQUAL(popcount16(RP),
               (epu8{2, 1, 0, 3, 4, 3, 2, 2, 1, 3, 2, 2, 3, 1, 1, 2}));
    EPU8_EQUAL(popcount16(RP << 1),
               (epu8{2, 1, 0, 3, 4, 3, 2, 2, 1, 3, 2, 2, 3, 1, 1, 2}));
    EPU8_EQUAL(popcount16(RP << 2),
               (epu8{2, 1, 0, 3, 4, 3, 2, 2, 1, 3, 2, 2, 3, 1, 1, 2}));
    EPU8_EQUAL(popcount16(Epu8({0, 1, 5, 0xff, 0xf0, 0x35}, 0x0f)),
               (Epu8({0, 1, 2, 8}, 4)));
}
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_random)
//****************************************************************************//

BOOST_AUTO_TEST_CASE(Random) {
    for (int i = 0; i < 10; i++) {
        epu8 r = random_epu8(255);
        EPU8_EQUAL(r, r);
    }
}
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_PermTransf16_test)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(IsPTransf, Fix) {
    BOOST_TEST(is_partial_transformation(zero));
    BOOST_TEST(is_partial_transformation(P01));
    BOOST_TEST(is_partial_transformation(P10));
    BOOST_TEST(not is_partial_transformation(Epu8({16}, 0)));
    BOOST_TEST(is_partial_transformation(Epu8({}, 0xff)));
    BOOST_TEST(is_partial_transformation(Epu8({2, 0xff, 3}, 0)));

    BOOST_TEST(not is_partial_transformation(zero, 15));
    BOOST_TEST(is_partial_transformation(Pa));
    BOOST_TEST(is_partial_transformation(Pa, 6));
    BOOST_TEST(is_partial_transformation(Pa, 5));
    BOOST_TEST(not is_partial_transformation(Pa, 4));
    BOOST_TEST(not is_partial_transformation(Pa, 1));
    BOOST_TEST(not is_partial_transformation(Pa, 0));

    BOOST_TEST(is_partial_transformation(RP));
    BOOST_TEST(is_partial_transformation(RP, 16));
    BOOST_TEST(not is_partial_transformation(RP, 15));
    BOOST_TEST(
        is_partial_transformation(Epu8({1, 2, 1, 0xFF, 0, 5, 0xFF, 2}, 0)));
    BOOST_TEST(not is_partial_transformation(
        Epu8({1, 2, 1, 0xFF, 0, 16, 0xFF, 2}, 0)));
}

BOOST_FIXTURE_TEST_CASE(IsTransf, Fix) {
    BOOST_TEST(is_transformation(zero));
    BOOST_TEST(is_transformation(P01));
    BOOST_TEST(is_transformation(P10));
    BOOST_TEST(not is_transformation(Epu8({16}, 0)));
    BOOST_TEST(not is_transformation(Epu8({}, 0xff)));
    BOOST_TEST(not is_transformation(Epu8({2, 0xff, 3}, 0)));

    BOOST_TEST(not is_transformation(zero, 15));
    BOOST_TEST(is_transformation(Pa));
    BOOST_TEST(is_transformation(Pa, 6));
    BOOST_TEST(is_transformation(Pa, 5));
    BOOST_TEST(not is_transformation(Pa, 4));
    BOOST_TEST(not is_transformation(Pa, 1));
    BOOST_TEST(not is_transformation(Pa, 0));

    BOOST_TEST(is_transformation(RP));
    BOOST_TEST(is_transformation(RP, 16));
    BOOST_TEST(not is_transformation(RP, 15));
}

BOOST_FIXTURE_TEST_CASE(IsPPerm, Fix) {
    BOOST_TEST(not is_partial_permutation(zero));
    BOOST_TEST(not is_partial_permutation(P01));
    BOOST_TEST(not is_partial_permutation(P10));
    BOOST_TEST(not is_partial_permutation(Epu8({16}, 0)));
    BOOST_TEST(is_partial_permutation(Epu8({}, 0xff)));
    BOOST_TEST(not is_partial_permutation(Epu8({2, 0xff, 3}, 0)));
    BOOST_TEST(is_partial_permutation(Epu8({2, 0xff, 3}, 0xff)));

    BOOST_TEST(not is_partial_permutation(zero, 15));
    BOOST_TEST(is_partial_permutation(Pa));
    BOOST_TEST(is_partial_permutation(Pa, 6));
    BOOST_TEST(is_partial_permutation(Pa, 5));
    BOOST_TEST(not is_partial_permutation(Pa, 4));
    BOOST_TEST(not is_partial_permutation(Pa, 1));
    BOOST_TEST(not is_partial_permutation(Pa, 0));

    BOOST_TEST(is_partial_permutation(RP));
    BOOST_TEST(is_partial_permutation(RP, 16));
    BOOST_TEST(not is_partial_permutation(RP, 15));

    BOOST_TEST(is_partial_permutation(
        epu8{1, 2, 0xFF, 0xFF, 0, 5, 0xFF, 3, 8, 9, 10, 11, 12, 13, 14, 15}));
    BOOST_TEST(not is_partial_permutation(
        epu8{1, 2, 1, 0xFF, 0, 5, 0xFF, 2, 8, 9, 10, 11, 12, 13, 14, 15}));
    BOOST_TEST(
        not is_partial_permutation(Epu8({1, 2, 1, 0xFF, 0, 5, 0xFF, 2}, 0)));
    BOOST_TEST(
        not is_partial_permutation(Epu8({1, 2, 1, 0xFF, 0, 16, 0xFF, 2}, 0)));
}

BOOST_FIXTURE_TEST_CASE(IsPerm, Fix) {
    BOOST_TEST(not is_permutation(zero));
    BOOST_TEST(not is_permutation(P01));
    BOOST_TEST(not is_permutation(P10));
    BOOST_TEST(not is_permutation(Epu8({16}, 0)));
    BOOST_TEST(not is_permutation(Epu8({}, 0xff)));
    BOOST_TEST(not is_permutation(Epu8({2, 0xff, 3}, 0)));

    BOOST_TEST(not is_permutation(zero, 15));
    BOOST_TEST(is_permutation(Pa));
    BOOST_TEST(is_permutation(Pa, 6));
    BOOST_TEST(is_permutation(Pa, 5));
    BOOST_TEST(not is_permutation(Pa, 4));
    BOOST_TEST(not is_permutation(Pa, 1));
    BOOST_TEST(not is_permutation(Pa, 0));

    BOOST_TEST(is_permutation(RP));
    BOOST_TEST(is_permutation(RP, 16));
    BOOST_TEST(not is_permutation(RP, 15));
}

BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//
