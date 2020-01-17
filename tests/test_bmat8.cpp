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

#define BOOST_TEST_MODULE BMAT8Tests

#include <boost/functional.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>

#include "bmat8.hpp"
#include "epu.hpp"
#include <iostream>

using namespace HPCombi;

#define EPU8_EQUAL(p1, p2) BOOST_CHECK_PREDICATE(equal, (p1)(p2))
#define EPU8_NOT_EQUAL(p1, p2)                                                 \
    BOOST_CHECK_PREDICATE(boost::not2(equal), (p1)(p2))

#define TEST_AGREES(type, ref, fun, vct)                                       \
    BOOST_FIXTURE_TEST_CASE(type##_agrees_##fun, Fix) {                        \
        for (type p : vct)                                                     \
            BOOST_TEST(p.fun() == p.ref());                                    \
    }
#define TEST_EPU8_AGREES(type, ref, fun, vct)                                  \
    BOOST_FIXTURE_TEST_CASE(type##_agrees_##fun, Fix) {                        \
        for (type p : vct)                                                     \
            EPU8_EQUAL(p.fun(), p.ref());                                      \
    }

#define TEST_AGREES2(type, ref, fun, vct)                                      \
    BOOST_FIXTURE_TEST_CASE(type##_agrees_##fun, Fix) {                        \
        for (type p1 : vct)                                                    \
            for (type p2 : vct)                                                \
                BOOST_TEST(p1.fun(p2) == p1.ref(p2));                          \
    }

struct Fix {
    Fix()
        : zero(0), one1(1), one2(0x201), ones(0xffffffffffffffff),
          bm({{0, 0, 0, 1, 0, 0, 1, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {0, 1, 1, 1, 0, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}}),
          bm1({{0, 0, 0, 1, 0, 0, 1, 1},
               {0, 0, 1, 0, 0, 1, 0, 1},
               {1, 1, 0, 0, 1, 1, 0, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 0, 1, 1, 1, 1, 1},
               {0, 1, 0, 1, 0, 1, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 0}}),
          bmm1({{1, 1, 0, 1, 0, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 0, 1, 1, 1, 1, 1},
                {0, 1, 1, 1, 0, 1, 1, 1},
                {0, 1, 1, 1, 0, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1}}),
          bm2({{1, 1}, {0, 1}}), bm2t({{1, 0}, {1, 1}}),
          bm3({{0, 0, 0, 1, 0, 0, 1, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {0, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 0}}),
          bm3t({{0, 1, 0, 1, 0, 1, 0, 0},
                {0, 1, 1, 1, 0, 1, 1, 1},
                {0, 1, 1, 0, 1, 0, 0, 1},
                {1, 1, 1, 1, 0, 0, 0, 1},
                {0, 1, 1, 1, 0, 0, 0, 1},
                {0, 1, 1, 1, 1, 0, 0, 0},
                {1, 0, 0, 1, 1, 0, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 0}}),
          BMlist(
              {zero, one1, one2, ones, bm, bm1, bmm1, bm2, bm2t, bm3, bm3t}) {
        BOOST_TEST_MESSAGE("setup fixture");
    }
    ~Fix() { BOOST_TEST_MESSAGE("teardown fixture"); }

    const BMat8 zero, one1, one2, ones, bm, bm1, bmm1, bm2, bm2t, bm3, bm3t;
    const std::vector<BMat8> BMlist;
};

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(BMat8_test)
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(BMat8_one, Fix) {
    BOOST_TEST(BMat8::one(0) == zero);
    BOOST_TEST(BMat8::one(2) == BMat8({{1, 0, 0, 0, 0, 0, 0, 0},
                                       {0, 1, 0, 0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 0}}));
    BOOST_TEST(BMat8::one(5) == BMat8({{1, 0, 0, 0, 0, 0, 0, 0},
                                       {0, 1, 0, 0, 0, 0, 0, 0},
                                       {0, 0, 1, 0, 0, 0, 0, 0},
                                       {0, 0, 0, 1, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 1, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 0}}));
    BOOST_TEST(BMat8::one(8) == BMat8::one());
}

BOOST_FIXTURE_TEST_CASE(BMat8_transpose, Fix) {
    BOOST_TEST(zero.transpose() == zero);
    BOOST_TEST(bm2.transpose() == bm2t);
    BOOST_TEST(bm3.transpose() == bm3t);

    for (auto m : BMlist)
        BOOST_TEST(m.transpose().transpose() == m);
}
TEST_AGREES(BMat8, transpose, transpose_mask, BMlist);
TEST_AGREES(BMat8, transpose, transpose_maskd, BMlist);

BOOST_FIXTURE_TEST_CASE(BMat8_transpose2, Fix) {
    for (auto a : BMlist) {
        for (auto b : BMlist) {
            BMat8 at = a, bt = b;
            BMat8::transpose2(at, bt);
            BOOST_TEST(at == a.transpose());
            BOOST_TEST(bt == b.transpose());
        }
    }
}
//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(BMat8_mult, Fix) {

    BMat8 tmp = bm * bm1;
    BOOST_TEST(tmp == bmm1);
    BOOST_TEST(tmp == bm * bm1);

    for (auto b : BMlist) {
        BOOST_TEST(zero * b == zero);
        BOOST_TEST(b * zero == zero);
        BOOST_TEST(b * b.one() == b);
        BOOST_TEST(b.one() * b == b);
        BOOST_TEST((b * b) * (b * b) == b * b * b * b);
    }

    for (auto b1 : BMlist)
        for (auto b2 : BMlist)
            for (auto b3 : BMlist)
                BOOST_TEST((b1 * b2) * b3 == b1 * (b2 * b3));
}

//****************************************************************************//
BOOST_AUTO_TEST_CASE(BMat8_random) {
    for (size_t d = 1; d < 8; ++d) {
        BMat8 bm = BMat8::random(d);
        for (size_t i = d + 1; i < 8; ++i) {
            for (size_t j = 0; j < 8; ++j) {
                BOOST_TEST(bm(i, j) == 0);
                BOOST_TEST(bm(j, i) == 0);
            }
        }
    }
}

//****************************************************************************//
BOOST_AUTO_TEST_CASE(BMat8_call_operator) {
    std::vector<std::vector<bool>> mat = {
        {0, 0, 0, 1, 0, 0, 1}, {0, 1, 1, 1, 0, 1, 0}, {1, 1, 0, 1, 1, 1, 1},
        {0, 0, 1, 0, 0, 1, 1}, {1, 1, 0, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 0, 1},
        {0, 1, 1, 1, 1, 0, 1}};
    BMat8 bm(mat);

    for (size_t i = 0; i < 7; ++i) {
        for (size_t j = 0; j < 7; ++j) {
            BOOST_TEST(static_cast<size_t>(bm(i, j)) == mat[i][j]);
        }
    }
}

//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(BMat8_operator_insert, Fix) {
    std::ostringstream oss;
    oss << bm3;
    BOOST_TEST(oss.str() == "00010011\n"
                            "11111101\n"
                            "01111101\n"
                            "11011111\n"
                            "00100111\n"
                            "11000001\n"
                            "01000011\n"
                            "01111010\n");

    std::stringbuf buff;
    std::ostream os(&buff);
    os << BMat8::random();  // Also does not do anything visible
}

//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(BMat8_set, Fix) {
    BMat8 bs;
    bs = bm;
    bs.set(0, 0, 1);
    BOOST_TEST(bs != bm);
    bs = bm;
    bs.set(0, 0, 0);
    BOOST_TEST(bs == bm);
    bs = bm;
    bs.set(2, 4, 1);
    BOOST_TEST(bs != bm);
    BOOST_TEST(bs == bm3);

    for (size_t i = 0; i < 8; ++i)
        for (size_t j = 0; j < 8; ++j)
            bs.set(i, j, true);
    BOOST_TEST(bs == ones);

    for (size_t i = 0; i < 8; ++i)
        for (size_t j = 0; j < 8; ++j)
            bs.set(i, j, false);
    BOOST_TEST(bs == zero);
}

//****************************************************************************//
BOOST_AUTO_TEST_CASE(BMat8_row_space_basis) {
    BMat8 bm({{0, 1, 1, 1, 0, 1, 0, 1},
              {0, 0, 0, 0, 0, 0, 0, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 bm2({{1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 1, 1, 1, 0, 1, 0},
               {0, 1, 1, 1, 0, 1, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {0, 0, 0, 0, 0, 0, 0, 1}});

    BOOST_TEST(bm.row_space_basis() == bm2.row_space_basis());

    BMat8 bm3({{1, 1, 1, 1, 0, 1, 0, 1},
               {0, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 0, 0, 1, 0, 1},
               {1, 1, 0, 0, 0, 1, 1, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {1, 0, 0, 0, 0, 1, 0, 0}});

    BMat8 bm4({{1, 1, 1, 1, 0, 1, 0, 1},
               {1, 1, 1, 0, 0, 1, 0, 1},
               {1, 0, 0, 0, 0, 1, 0, 0},
               {0, 1, 1, 1, 1, 1, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 0, 0, 0, 0, 0, 0, 0},
               {0, 0, 0, 0, 0, 0, 0, 0},
               {0, 0, 0, 0, 0, 0, 0, 0}});

    BOOST_TEST(bm3.row_space_basis() == bm4);
    BOOST_TEST(bm4.row_space_basis() == bm4);

    BMat8 bm5(0xff00000000000000);

    uint64_t data = 0xffffffffffffffff;

    for (size_t i = 0; i < 7; ++i) {
        BOOST_TEST(BMat8(data).row_space_basis() == bm5);
        data = data >> 8;
    }

    for (size_t i = 0; i < 1000; ++i) {
        bm = BMat8::random();
        BOOST_TEST(bm.row_space_basis().row_space_basis() ==
                   bm.row_space_basis());
    }
}

//****************************************************************************//
//****************************************************************************//
BOOST_AUTO_TEST_CASE(BMat8_col_space_basis) {
    BMat8 bm({{0, 1, 1, 1, 0, 1, 0, 1},
              {0, 0, 0, 0, 0, 0, 0, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 bm2({{1, 1, 1, 1, 1, 0, 0, 0},
               {1, 0, 0, 0, 0, 0, 0, 0},
               {1, 1, 1, 1, 1, 1, 1, 0},
               {1, 1, 1, 1, 0, 1, 1, 1},
               {1, 1, 0, 0, 1, 0, 0, 1},
               {1, 0, 1, 0, 0, 1, 0, 0},
               {1, 0, 1, 0, 0, 0, 0, 1},
               {0, 0, 1, 1, 1, 0, 1, 1}});

    BOOST_TEST(bm.col_space_basis() == bm2);

    BMat8 bm3({{1, 1, 1, 1, 0, 1, 0, 1},
               {0, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 0, 0, 1, 0, 1},
               {1, 1, 0, 0, 0, 1, 1, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {1, 0, 0, 0, 0, 1, 0, 0}});

    BMat8 bm4({{1, 1, 1, 0, 0, 0, 0, 0},
               {1, 1, 0, 1, 0, 0, 0, 0},
               {1, 1, 1, 1, 0, 0, 0, 0},
               {1, 1, 1, 1, 0, 0, 0, 0},
               {1, 0, 1, 0, 0, 0, 0, 0},
               {0, 0, 1, 0, 1, 0, 0, 0},
               {0, 0, 0, 0, 1, 0, 0, 0},
               {0, 0, 1, 0, 0, 0, 0, 0}});

    BOOST_TEST(bm3.col_space_basis() == bm4);

    uint64_t col = 0x8080808080808080;
    BMat8 bm5(col);

    uint64_t data = 0xffffffffffffffff;

    for (size_t i = 0; i < 7; ++i) {
        BOOST_TEST(BMat8(data).col_space_basis() == bm5);
        data &= ~(col >> i);
    }

    for (size_t i = 0; i < 1000; ++i) {
        bm = BMat8::random();
        BOOST_TEST(bm.col_space_basis().col_space_basis() ==
                   bm.col_space_basis());
    }
}

//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(BMat8_row_space_size, Fix) {
    BOOST_TEST(zero.row_space_size() == 1);
    BOOST_TEST(one1.row_space_size() == 2);
    BOOST_TEST(one2.row_space_size() == 4);
    BOOST_TEST(BMat8::one().row_space_size() == 256);
    BOOST_TEST(bm.row_space_size() == 22);
    BOOST_TEST(bm1.row_space_size() == 31);
    BOOST_TEST(bm2.row_space_size() == 3);
    BOOST_TEST(bm2t.row_space_size() == 3);
    BOOST_TEST(bm3.row_space_size() == 21);
    BOOST_TEST(bm3t.row_space_size() == 21);
    BOOST_TEST(bmm1.row_space_size() == 6);
}
TEST_AGREES(BMat8, row_space_size_ref, row_space_size, BMlist);
TEST_AGREES(BMat8, row_space_size_ref, row_space_size_incl, BMlist);
TEST_AGREES(BMat8, row_space_size_ref, row_space_size_incl1, BMlist);
TEST_AGREES(BMat8, row_space_size_ref, row_space_size_bitset, BMlist);

//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(BMat8_row_space_included, Fix) {
    BOOST_TEST(zero.row_space_included(one1));
    BOOST_TEST(not one1.row_space_included(zero));

    BMat8 m1({{1, 1, 0}, {1, 0, 1}, {0, 0, 0}});
    BMat8 m2({{0, 0, 0}, {1, 0, 1}, {1, 1, 0}});
    BOOST_TEST(m1.row_space_included(m2));
    BOOST_TEST(m2.row_space_included(m1));

    BMat8 m3({{0, 0, 1}, {1, 0, 1}, {1, 1, 0}});
    BOOST_TEST(m1.row_space_included(m3));
    BOOST_TEST(m2.row_space_included(m3));
    BOOST_TEST(not m3.row_space_included(m1));
    BOOST_TEST(not m3.row_space_included(m1));

    BOOST_TEST(m1.row_space_included(BMat8::one()));
    BOOST_TEST(m2.row_space_included(BMat8::one()));
    BOOST_TEST(m3.row_space_included(BMat8::one()));
}
TEST_AGREES2(BMat8, row_space_included, row_space_included_ref, BMlist);
TEST_AGREES2(BMat8, row_space_included, row_space_included_bitset, BMlist);

BOOST_FIXTURE_TEST_CASE(BMat8_row_space_included2, Fix) {
    BMat8 a0 = BMat8::one();
    BMat8 b0 = BMat8(0);
    BMat8 a1 = BMat8(0);
    BMat8 b1 = BMat8::one();

    auto res = BMat8::row_space_included2(a0, b0, a1, b1);
    BOOST_TEST(res.first == a0.row_space_included(b0));
    BOOST_TEST(res.second == a1.row_space_included(b1));

    for (auto a0 : BMlist) {
        for (auto b0 : BMlist) {
            for (auto a1 : BMlist) {
                for (auto b1 : BMlist) {
                    auto res = BMat8::row_space_included2(a0, b0, a1, b1);
                    BOOST_TEST(res.first == a0.row_space_included(b0));
                    BOOST_TEST(res.second == a1.row_space_included(b1));
                }
            }
        }
    }
}

//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(BMat8_row_permuted, Fix) {
    BOOST_TEST(bm2.row_permuted(Perm16({1, 0})) == BMat8({{0, 1}, {1, 1}}));
    BOOST_TEST(bm2.row_permuted(Perm16({2, 1, 0})) ==
               BMat8({{0, 0, 0}, {0, 1, 0}, {1, 1, 0}}));
    BOOST_TEST(bm.row_permuted(Perm16({5, 3, 1, 4, 2, 0})) ==
               BMat8({{1, 1, 0, 0, 0, 0, 0, 1},
                      {1, 1, 0, 1, 1, 1, 1, 1},
                      {1, 1, 1, 1, 1, 1, 0, 1},
                      {0, 0, 1, 0, 0, 1, 1, 1},
                      {0, 1, 1, 1, 0, 1, 0, 1},
                      {0, 0, 0, 1, 0, 0, 1, 1},
                      {0, 1, 0, 0, 0, 0, 1, 1},
                      {0, 1, 1, 1, 1, 0, 1, 0}}));
    BOOST_TEST(BMat8::one().row_permuted(Perm16({5, 3, 1, 4, 2, 0})) ==
               BMat8({{0, 0, 0, 0, 0, 1, 0, 0},
                      {0, 0, 0, 1, 0, 0, 0, 0},
                      {0, 1, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 1, 0, 0, 0},
                      {0, 0, 1, 0, 0, 0, 0, 0},
                      {1, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 1, 0},
                      {0, 0, 0, 0, 0, 0, 0, 1}}));
}

BOOST_FIXTURE_TEST_CASE(BMat8_col_permuted, Fix) {
    BOOST_TEST(bm2.col_permuted(Perm16({1, 0})) == BMat8({{1, 1}, {1, 0}}));
    BOOST_TEST(bm2.col_permuted(Perm16({2, 1, 0})) ==
               BMat8({{0, 1, 1}, {0, 1, 0}, {0, 0, 0}}));
    BOOST_TEST(bm.col_permuted(Perm16({5, 3, 1, 4, 2, 0})) ==
               BMat8({{0, 1, 0, 0, 0, 0, 1, 1},
                      {1, 1, 1, 1, 1, 1, 0, 1},
                      {1, 1, 1, 0, 1, 0, 0, 1},
                      {1, 1, 1, 1, 0, 1, 1, 1},
                      {1, 0, 0, 0, 1, 0, 1, 1},
                      {0, 0, 1, 0, 0, 1, 0, 1},
                      {0, 0, 1, 0, 0, 0, 1, 1},
                      {0, 1, 1, 1, 1, 0, 1, 0}}));
    BOOST_TEST(BMat8::one().col_permuted(Perm16({4, 1, 3, 0, 2, 6, 5})) ==
               BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                      {0, 1, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 1, 0, 0, 0},
                      {0, 0, 1, 0, 0, 0, 0, 0},
                      {1, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 1, 0},
                      {0, 0, 0, 0, 0, 1, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 1}}));
}

BOOST_FIXTURE_TEST_CASE(BMat8_row_permutation_matrix, Fix) {
    BOOST_TEST(BMat8::row_permutation_matrix(Perm16({1, 0})) ==
               BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                      {1, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 1, 0, 0, 0, 0, 0},
                      {0, 0, 0, 1, 0, 0, 0, 0},
                      {0, 0, 0, 0, 1, 0, 0, 0},
                      {0, 0, 0, 0, 0, 1, 0, 0},
                      {0, 0, 0, 0, 0, 0, 1, 0},
                      {0, 0, 0, 0, 0, 0, 0, 1}}));
    BOOST_TEST(BMat8::row_permutation_matrix(Perm16({1, 3, 4, 0, 2})) ==
               BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 1, 0, 0, 0, 0},
                      {0, 0, 0, 0, 1, 0, 0, 0},
                      {1, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 1, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 1, 0, 0},
                      {0, 0, 0, 0, 0, 0, 1, 0},
                      {0, 0, 0, 0, 0, 0, 0, 1}}));
    BOOST_TEST(BMat8::row_permutation_matrix(Perm16({5, 3, 1, 4, 2, 0})) ==
               BMat8({{0, 0, 0, 0, 0, 1, 0, 0},
                      {0, 0, 0, 1, 0, 0, 0, 0},
                      {0, 1, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 1, 0, 0, 0},
                      {0, 0, 1, 0, 0, 0, 0, 0},
                      {1, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 1, 0},
                      {0, 0, 0, 0, 0, 0, 0, 1}}));
}

BOOST_FIXTURE_TEST_CASE(BMat8_col_permutation_matrix, Fix) {
    BOOST_TEST(BMat8::col_permutation_matrix(Perm16({1, 0})) ==
               BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                      {1, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 1, 0, 0, 0, 0, 0},
                      {0, 0, 0, 1, 0, 0, 0, 0},
                      {0, 0, 0, 0, 1, 0, 0, 0},
                      {0, 0, 0, 0, 0, 1, 0, 0},
                      {0, 0, 0, 0, 0, 0, 1, 0},
                      {0, 0, 0, 0, 0, 0, 0, 1}}));
    BOOST_TEST(BMat8::col_permutation_matrix(Perm16({1, 3, 4, 0, 2})) ==
               BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                      {1, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 1, 0, 0, 0},
                      {0, 1, 0, 0, 0, 0, 0, 0},
                      {0, 0, 1, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 1, 0, 0},
                      {0, 0, 0, 0, 0, 0, 1, 0},
                      {0, 0, 0, 0, 0, 0, 0, 1}}));
    BOOST_TEST(BMat8::col_permutation_matrix(Perm16({5, 3, 1, 4, 2, 0})) ==
               BMat8({{0, 0, 0, 0, 0, 1, 0, 0},
                      {0, 0, 1, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 1, 0, 0, 0},
                      {0, 1, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 1, 0, 0, 0, 0},
                      {1, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 1, 0},
                      {0, 0, 0, 0, 0, 0, 0, 1}}));
}

//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(BMat8_nr_rows, Fix) {
    BOOST_TEST(zero.nr_rows() == 0);
    BOOST_TEST(one1.nr_rows() == 1);
    BOOST_TEST(one2.nr_rows() == 2);
    BOOST_TEST(bm.nr_rows() == 8);
    BOOST_TEST(BMat8({{1, 0, 1}, {1, 1, 0}, {0, 0, 0}}).nr_rows() == 2);
}

//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(BMat8_right_perm_action_on_basis_ref, Fix) {
    BMat8 m1({{1, 1, 0}, {1, 0, 1}, {0, 0, 0}});
    BMat8 m2({{0, 0, 0}, {1, 0, 1}, {1, 1, 0}});
    BOOST_TEST(m1.right_perm_action_on_basis_ref(m2) == Perm16({1, 0}));
    BOOST_TEST(m1.right_perm_action_on_basis(m2) == Perm16({1, 0}));

    m1 = BMat8({{1, 1, 0, 1}, {1, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}});
    m2 = BMat8({{1, 0, 0, 0}, {0, 1, 0, 1}, {1, 0, 1, 0}, {0, 0, 0, 1}});
    BOOST_TEST(m1.right_perm_action_on_basis_ref(m2) == Perm16::one());
    BOOST_TEST(m1.right_perm_action_on_basis(m2) == Perm16::one());

    m1 = BMat8({{1, 1, 0, 1}, {1, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}});
    m2 = BMat8({{0, 0, 0, 0}, {1, 1, 0, 1}, {1, 0, 1, 0}, {0, 0, 0, 1}});
    BOOST_TEST(m1.right_perm_action_on_basis_ref(m2) == Perm16::one());
    BOOST_TEST(m1.right_perm_action_on_basis(m2) == Perm16::one());

    m1 = BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}, {0, 0, 0, 0}});
    m2 = BMat8({{1, 0, 0, 1}, {0, 0, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 1}});
    BOOST_TEST(m1.right_perm_action_on_basis_ref(m2) == Perm16({1, 0}));
    BOOST_TEST(m1.right_perm_action_on_basis(m2) == Perm16({1, 0}));

    m1 = BMat8({{0, 0, 0, 1}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 0, 0}});
    m2 = BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 1}});
    BOOST_TEST(m1.right_perm_action_on_basis_ref(m2) == Perm16({0, 2, 3, 1}));
    BOOST_TEST(m1.right_perm_action_on_basis(m2) == Perm16({0, 2, 3, 1}));

    m1 = BMat8({{0, 0, 0, 1}, {0, 0, 1, 0}, {0, 1, 0, 0}, {1, 0, 0, 0}});
    m2 = BMat8({{0, 1, 0, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}, {0, 0, 1, 0}});
    BOOST_TEST(m1.right_perm_action_on_basis_ref(m2) == Perm16({2, 0, 3, 1}));
    BOOST_TEST(m1.right_perm_action_on_basis(m2) == Perm16({2, 0, 3, 1}));
}
//****************************************************************************//
BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//
