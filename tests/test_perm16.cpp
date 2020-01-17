/******************************************************************************/
/*       Copyright (C) 2017 Florent Hivert <Florent.Hivert@lri.fr>,           */
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

#define BOOST_TEST_MODULE Perm16Tests

#include "perm16.hpp"
#include <boost/test/unit_test.hpp>

using HPCombi::epu8;
using HPCombi::Epu8;
using HPCombi::equal;
using HPCombi::is_partial_transformation;
using HPCombi::is_permutation;
using HPCombi::is_transformation;

using HPCombi::Perm16;
using HPCombi::PPerm16;
using HPCombi::PTransf16;
using HPCombi::Transf16;

const uint8_t FF = 0xff;

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

std::vector<Perm16> all_perms(uint8_t sz) {
    std::vector<Perm16> res{};
    epu8 x = HPCombi::epu8id;
    res.push_back(x);
    auto &refx = HPCombi::as_array(x);
    while (std::next_permutation(refx.begin(), refx.begin() + sz)) {
        res.push_back(x);
    }
    return res;
};

std::vector<PPerm16> all_pperms(std::vector<Perm16> perms,
                                std::vector<epu8> masks) {
    std::vector<PPerm16> res{};
    for (epu8 mask : masks) {
        for (Perm16 p : perms) {
            res.push_back(p.v | mask);
        }
    }
    return res;
}

struct Fix {
    Fix()
        : zero(Epu8({}, 0)), P01(Epu8({0, 1}, 0)), P10(Epu8({1, 0}, 0)),
          P11(Epu8({1, 1}, 0)), P1(Epu8({}, 1)),
          RandT({3, 1, 0, 14, 15, 13, 5, 10, 2, 11, 6, 12, 7, 4, 8, 9}),
          PPa({1, 2, 3, 4, 0, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
          PPb({1, 2, 3, 6, 0, 5, 4, 7, 8, 9, 10, 11, 12, 15, 14, 13}),
          RandPerm(RandT),
          Tlist({zero, P01, P10, P11, P1, RandT, epu8(PPa), epu8(PPb)}),
          PlistSmall(all_perms(6)), Plist(all_perms(9)),
          PPmasks({Epu8(0), Epu8(FF), Epu8({0}, FF), Epu8({0, 0}, FF),
                   Epu8({0, FF, 0}, FF), Epu8({0, FF, 0}, 0),
                   Epu8({0, FF, 0, FF, 0, 0, 0, FF, FF}, 0)}),
          PPlist(all_pperms(PlistSmall, PPmasks)) {
        BOOST_TEST_MESSAGE("setup fixture");
    }
    ~Fix() { BOOST_TEST_MESSAGE("teardown fixture"); }

    const Transf16 zero, P01, P10, P11, P1, RandT;
    const Perm16 PPa, PPb, RandPerm;
    const std::vector<Transf16> Tlist;
    const std::vector<Perm16> PlistSmall, Plist;
    const std::vector<epu8> PPmasks;
    const std::vector<PPerm16> PPlist;
};

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(PTransf16_test)
//****************************************************************************//

BOOST_AUTO_TEST_CASE(PTransf16_constructor) {
    const uint8_t FF = 0xff;
    BOOST_TEST(PTransf16({}) == PTransf16::one());
    BOOST_TEST(PTransf16({0, 1, 2, 3}) == PTransf16::one());
    BOOST_TEST(PTransf16({1, 0}) == PTransf16({1, 0, 2}));
    BOOST_TEST(PTransf16({2}) == PTransf16({2, 1, 2}));
    BOOST_TEST(PTransf16({4, 5, 0}, {9, 0, 1}) ==
               PTransf16({1, FF, FF, FF, 9, 0, FF, FF, FF, FF, FF, FF, FF, FF,
                          FF, FF}));
    BOOST_TEST(PTransf16({4, 5, 0, 8}, {9, 0, 1, 2}) ==
               PTransf16({1, FF, FF, FF, 9, 0, FF, FF, 2, FF, FF, FF, FF, FF,
                          FF, FF}));
    BOOST_TEST(PTransf16({4, 5, 0, 8}, {9, 0, 2, 2}) ==
               PTransf16({2, FF, FF, FF, 9, 0, FF, FF, 2, FF, FF, FF, FF, FF,
                          FF, FF}));
}

BOOST_AUTO_TEST_CASE(PTransf16_hash) {
    BOOST_TEST(std::hash<PTransf16>()(PTransf16::one()) != 0);
    BOOST_TEST(std::hash<PTransf16>()(PTransf16(Epu8(1))) != 0);
    BOOST_TEST(std::hash<PTransf16>()(PTransf16({4, 5, 0}, {9, 0, 1})) != 0);
}

BOOST_AUTO_TEST_CASE(PTransf16_image_mask) {
    EPU8_EQUAL(PTransf16({}).image_mask(), Epu8(FF));
    EPU8_EQUAL(PTransf16({}).image_mask(false), Epu8(FF));
    EPU8_EQUAL(PTransf16({}).image_mask(true), Epu8(0));
    EPU8_EQUAL(PTransf16({4, 4, 4, 4}).image_mask(), Epu8({0, 0, 0, 0}, FF));
    EPU8_EQUAL(PTransf16({4, 4, 4, 4}).image_mask(false),
               Epu8({0, 0, 0, 0}, FF));
    EPU8_EQUAL(PTransf16({4, 4, 4, 4}).image_mask(true),
               Epu8({FF, FF, FF, FF}, 0));
    EPU8_EQUAL(PTransf16(Epu8(1)).image_mask(), Epu8({0, FF}, 0));
    EPU8_EQUAL(PTransf16(Epu8(2)).image_mask(), Epu8({0, 0, FF}, 0));
    EPU8_EQUAL(PTransf16(Epu8({2, 2, 2, 0xf}, 2)).image_mask(),
               Epu8({0, 0, FF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, FF}, 0));
    EPU8_EQUAL(
        PTransf16(Epu8({0, 2, 2, 0xf, 2, 2, 2, 2, 5, 2}, 2)).image_mask(),
        Epu8({FF, 0, FF, 0, 0, FF, 0, 0, 0, 0, 0, 0, 0, 0, 0, FF}, 0));
    EPU8_EQUAL(
        PTransf16(Epu8({0, 2, 2, 0xf, 2, 2, 2, 2, 5, 2}, 2)).image_mask(false),
        Epu8({FF, 0, FF, 0, 0, FF, 0, 0, 0, 0, 0, 0, 0, 0, 0, FF}, 0));
    EPU8_EQUAL(
        PTransf16(Epu8({0, 2, 2, 0xf, 2, 2, 2, 2, 5, 2}, 2)).image_mask(true),
        Epu8({0, FF, 0, FF, FF, 0, FF, FF, FF, FF, FF, FF, FF, FF, FF, 0}, 0));
}

BOOST_AUTO_TEST_CASE(PTransf16_left_one) {
    BOOST_TEST(PTransf16({}).left_one() == PTransf16::one());
    BOOST_TEST(PTransf16({4, 4, 4, 4}).left_one() ==
               PTransf16({FF, FF, FF, FF}));
    BOOST_TEST(PTransf16(Epu8(1)).left_one() == PTransf16(Epu8({FF, 1}, FF)));
    BOOST_TEST(PTransf16(Epu8(2)).left_one() ==
               PTransf16(Epu8({FF, FF, 2}, FF)));
    BOOST_TEST(PTransf16(Epu8({2, 2, 2, 0xf}, 2)).left_one() ==
               PTransf16({FF, FF, 2, FF, FF, FF, FF, FF, FF, FF, FF, FF, FF, FF,
                          FF, 15}));
    BOOST_TEST(PTransf16(Epu8({FF, 2, 2, 0xf}, FF)).left_one() ==
               PTransf16({FF, FF, 2, FF, FF, FF, FF, FF, FF, FF, FF, FF, FF, FF,
                          FF, 15}));
    BOOST_TEST(
        PTransf16(Epu8({0, 2, 2, 0xf, 2, 2, 2, 2, 5, 2}, 2)).left_one() ==
        PTransf16(
            {0, FF, 2, FF, FF, 5, FF, FF, FF, FF, FF, FF, FF, FF, FF, 15}));
    BOOST_TEST(
        PTransf16(Epu8({0, 2, FF, 0xf, 2, FF, 2, FF, 5}, FF)).left_one() ==
        PTransf16(
            {0, FF, 2, FF, FF, 5, FF, FF, FF, FF, FF, FF, FF, FF, FF, 15}));
}

BOOST_AUTO_TEST_CASE(PTransf16_domain_mask) {
    EPU8_EQUAL(PTransf16({}).domain_mask(), Epu8(FF));
    EPU8_EQUAL(PTransf16({4, 4, 4, 4}).domain_mask(), Epu8(FF));
    EPU8_EQUAL(PTransf16({4, 4, 4, 4}).domain_mask(false), Epu8(FF));
    EPU8_EQUAL(PTransf16({4, 4, 4, 4}).domain_mask(true), Epu8(0));
    EPU8_EQUAL(PTransf16(Epu8(1)).domain_mask(), Epu8(FF));
    EPU8_EQUAL(PTransf16(Epu8(2)).domain_mask(), Epu8(FF));
    EPU8_EQUAL(PTransf16(Epu8({2, 2, 2, 0xf}, FF)).domain_mask(),
               Epu8({FF, FF, FF, FF}, 0));
    EPU8_EQUAL(PTransf16(Epu8({FF, 2, 2, 0xf}, FF)).domain_mask(),
               Epu8({0, FF, FF, FF}, 0));
    EPU8_EQUAL(
        PTransf16(Epu8({0, 2, FF, 0xf, 2, FF, 2, FF, 5}, FF)).domain_mask(),
        Epu8({FF, FF, 0, FF, FF, 0, FF, 0, FF}, 0));
    EPU8_EQUAL(PTransf16(Epu8({0, 2, FF, 0xf, 2, FF, 2, FF, 5}, FF))
                   .domain_mask(false),
               Epu8({FF, FF, 0, FF, FF, 0, FF, 0, FF}, 0));
    EPU8_EQUAL(
        PTransf16(Epu8({0, 2, FF, 0xf, 2, FF, 2, FF, 5}, FF)).domain_mask(true),
        Epu8({0, 0, FF, 0, 0, FF, 0, FF, 0}, FF));
}

BOOST_AUTO_TEST_CASE(PTransf16_right_one) {
    BOOST_TEST(PTransf16({}).right_one() == PTransf16::one());
    BOOST_TEST(PTransf16({4, 4, 4, 4}).right_one() == PTransf16::one());
    BOOST_TEST(PTransf16(Epu8(1)).right_one() == PTransf16::one());
    BOOST_TEST(PTransf16(Epu8(2)).right_one() == PTransf16::one());
    BOOST_TEST(PTransf16(Epu8({2, 2, 2, 0xf}, FF)).right_one() ==
               PTransf16(Epu8({0, 1, 2, 3}, FF)));
    BOOST_TEST(PTransf16(Epu8({FF, 2, 2, 0xf}, FF)).right_one() ==
               PTransf16({FF, 1, 2, 3, FF, FF, FF, FF, FF, FF, FF, FF, FF, FF,
                          FF, FF}));
    BOOST_TEST(
        PTransf16(Epu8({0, 2, 2, 0xf, 2, 2, 2, 2, 5, 2}, 2)).right_one() ==
        PTransf16::one());
    BOOST_TEST(
        PTransf16(Epu8({0, 2, FF, 0xf, 2, FF, 2, FF, 5}, FF)).right_one() ==
        PTransf16({0, 1, FF, 3, 4, FF, 6, FF, 8, FF, FF, FF, FF, FF, FF, FF}));
}

BOOST_AUTO_TEST_CASE(PTransf16_rank_ref) {
    BOOST_TEST(PTransf16({}).rank_ref() == 16);
    BOOST_TEST(PTransf16({4, 4, 4, 4}).rank() == 12);
    BOOST_TEST(PTransf16({1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1})
                   .rank_ref() == 1);
    BOOST_TEST(PTransf16({2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2})
                   .rank_ref() == 1);
    BOOST_TEST(PTransf16({2, 2, 2, 0xf, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2})
                   .rank_ref() == 2);
    BOOST_TEST(PTransf16({0, 2, 2, 0xf, 2, 2, 2, 2, 5, 2, 2, 2, 2, 2, 2, 2})
                   .rank_ref() == 4);
    BOOST_TEST(PTransf16({1, 1, 1, FF, 1, 1, FF, 1, 1, FF, 1, FF, 1, 1, 1, 1})
                   .rank_ref() == 1);
    BOOST_TEST(PTransf16({2, 2, 2, 2, 2, FF, 2, 2, 2, FF, 2, 2, 2, FF, 2, 2})
                   .rank_ref() == 1);
    BOOST_TEST(PTransf16({2, 2, 2, 0xf, 2, FF, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2})
                   .rank_ref() == 2);
    BOOST_TEST(PTransf16({0, 2, 2, 0xf, 2, 2, FF, 2, 5, 2, FF, 2, 2, 2, 2, 2})
                   .rank_ref() == 4);
}

BOOST_AUTO_TEST_CASE(PTransf16_rank) {
    BOOST_TEST(PTransf16({}).rank() == 16);
    BOOST_TEST(PTransf16({4, 4, 4, 4}).rank() == 12);
    BOOST_TEST(
        PTransf16({1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}).rank() ==
        1);
    BOOST_TEST(
        PTransf16({2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}).rank() ==
        1);
    BOOST_TEST(
        PTransf16({2, 2, 2, 0xf, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}).rank() ==
        2);
    BOOST_TEST(
        PTransf16({0, 2, 2, 0xf, 2, 2, 2, 2, 5, 2, 2, 2, 2, 2, 2, 2}).rank() ==
        4);
}

BOOST_AUTO_TEST_CASE(PTransf16_fix_points_mask) {
    EPU8_EQUAL(PTransf16({}).fix_points_mask(), Epu8(FF));
    EPU8_EQUAL(PTransf16({}).fix_points_mask(false), Epu8(FF));
    EPU8_EQUAL(PTransf16({}).fix_points_mask(true), Epu8(0));
    EPU8_EQUAL(PTransf16({4, 4, 4, 4}).fix_points_mask(),
               Epu8({0, 0, 0, 0}, FF));
    EPU8_EQUAL(PTransf16({4, 4, 4, 4}).fix_points_mask(false),
               Epu8({0, 0, 0, 0}, FF));
    EPU8_EQUAL(PTransf16({4, 4, 4, 4}).fix_points_mask(true),
               Epu8({FF, FF, FF, FF}, 0));
    EPU8_EQUAL(PTransf16(Epu8(1)).fix_points_mask(), Epu8({0, FF}, 0));
    EPU8_EQUAL(PTransf16(Epu8(2)).fix_points_mask(), Epu8({0, 0, FF}, 0));
    EPU8_EQUAL(PTransf16(Epu8({2, 2, 2, 0xf}, 7)).fix_points_mask(),
               Epu8({0, 0, FF, 0, 0, 0, 0, FF, 0, 0, 0, 0, 0, 0, 0, 0}, 0));
    EPU8_EQUAL(
        PTransf16(Epu8({0, 2, 2, 0xf, 2, 2, 2, 14, 5, 2}, 2)).fix_points_mask(),
        Epu8({FF, 0, FF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0));
    EPU8_EQUAL(PTransf16(Epu8({0, 2, 2, 0xf, 2, 2, 2, 2, 8, 2}, 14))
                   .fix_points_mask(false),
               Epu8({FF, 0, FF, 0, 0, 0, 0, 0, FF, 0, 0, 0, 0, 0, FF, 0}, 0));
    EPU8_EQUAL(PTransf16(Epu8({0, 2, 2, 0xf, 2, 2, 2, 2, 5, 2}, 2))
                   .fix_points_mask(true),
               Epu8({0, FF, 0}, FF));
}
BOOST_AUTO_TEST_CASE(PTransf16_fix_points_bitset) {
    BOOST_TEST(PTransf16({}).fix_points_bitset() == 0xFFFF);
    BOOST_TEST(PTransf16({}).fix_points_bitset(false) == 0xFFFF);
    BOOST_TEST(PTransf16({}).fix_points_bitset(true) == 0);
    BOOST_TEST(PTransf16({4, 4, 4, 4}).fix_points_bitset() == 0xFFF0);
    BOOST_TEST(PTransf16({4, 4, 4, 4}).fix_points_bitset(false) == 0xFFF0);
    BOOST_TEST(PTransf16({4, 4, 4, 4}).fix_points_bitset(true) == 0x000F);
    BOOST_TEST(PTransf16(Epu8(1)).fix_points_bitset() == 0x0002);
    BOOST_TEST(PTransf16(Epu8(2)).fix_points_bitset() == 0x0004);
    BOOST_TEST(PTransf16(Epu8({2, 2, 2, 0xf}, 7)).fix_points_bitset() ==
               0x0084);
    BOOST_TEST(PTransf16(Epu8({0, 2, 2, 0xf, 2, 2, 2, 14, 5, 2}, 2))
                   .fix_points_bitset() == 0x5);
    BOOST_TEST(PTransf16(Epu8({0, 2, 2, 0xf, 2, 2, 2, 2, 8, 2}, 14))
                   .fix_points_bitset(false) == 0x4105);
    BOOST_TEST(PTransf16(Epu8({0, 2, 2, 0xf, 2, 2, 2, 2, 5, 2}, 2))
                   .fix_points_bitset(true) == 0xFFFA);
}
BOOST_AUTO_TEST_CASE(PTransf16_nb_fix_points) {
    BOOST_TEST(PTransf16({}).nb_fix_points() == 16);
    BOOST_TEST(PTransf16({4, 4, 4, 4}).nb_fix_points() == 12);
    BOOST_TEST(PTransf16(Epu8(1)).nb_fix_points() == 1);
    BOOST_TEST(PTransf16(Epu8(2)).nb_fix_points() == 1);
    BOOST_TEST(PTransf16(Epu8({2, 2, 2, 0xf}, 7)).nb_fix_points() == 2);
    BOOST_TEST(
        PTransf16(Epu8({0, 2, 2, 0xf, 2, 2, 2, 14, 5, 2}, 2)).nb_fix_points() ==
        2);
    BOOST_TEST(
        PTransf16(Epu8({0, 2, 2, 0xf, 2, 2, 2, 2, 8, 2}, 14)).nb_fix_points() ==
        4);
}

BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(Transf16_test)
//****************************************************************************//

BOOST_FIXTURE_TEST_CASE(Transf16OperatorUInt64, Fix) {
    BOOST_TEST(static_cast<uint64_t>(Transf16::one()) == 0xf7e6d5c4b3a29180);
    BOOST_TEST(static_cast<uint64_t>(zero) == 0x0);
    BOOST_TEST(static_cast<uint64_t>(P10) == 0x1);
    BOOST_TEST(static_cast<uint64_t>(P01) == 0x100);
    BOOST_TEST(static_cast<uint64_t>(P11) == 0x101);
    BOOST_TEST(static_cast<uint64_t>(P1) == 0x1111111111111111);
    BOOST_TEST(static_cast<uint64_t>(RandT) == 0x9a854d7fce60b123);
}

BOOST_FIXTURE_TEST_CASE(Transf16ConstrUInt64, Fix) {
    BOOST_TEST(static_cast<Transf16>(0x0) == zero);
    BOOST_TEST(static_cast<Transf16>(0x1) == P10);
    BOOST_TEST(static_cast<Transf16>(0x100) == P01);
    for (auto p : Tlist)
        BOOST_TEST(static_cast<Transf16>(static_cast<uint64_t>(p)) == p);
}

BOOST_FIXTURE_TEST_CASE(Transf16_hash, Fix) {
    BOOST_TEST(std::hash<Transf16>()(Transf16::one()) != 0);
    BOOST_TEST(std::hash<Transf16>()(Transf16(Epu8(1))) != 0);
    BOOST_TEST(std::hash<Transf16>()(RandT) != 0);
}

BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(Perm16_constr)
//****************************************************************************//

BOOST_FIXTURE_TEST_CASE(Perm16OperatorUInt64, Fix) {
    BOOST_TEST(static_cast<uint64_t>(Perm16::one()) == 0xf7e6d5c4b3a29180);
    BOOST_TEST(static_cast<uint64_t>(PPa) == 0xf7e6d5c0b4a39281);
    BOOST_TEST(static_cast<uint64_t>(PPb) == 0xd7e4f5c0b6a39281);
    BOOST_TEST(static_cast<uint64_t>(RandPerm) == 0x9a854d7fce60b123);

    for (auto p : {Perm16::one(), PPa, PPb, RandPerm})
        BOOST_TEST(static_cast<Perm16>(static_cast<uint64_t>(p)) == p);
}

BOOST_AUTO_TEST_CASE(Perm16TestEq) {
    BOOST_TEST(Perm16::one() * Perm16::one() == Perm16::one());
}

BOOST_FIXTURE_TEST_CASE(Perm16_hash, Fix) {
    BOOST_TEST(std::hash<Perm16>()(Transf16::one()) != 0);
    BOOST_TEST(std::hash<Perm16>()(PPa) != 0);
    BOOST_TEST(std::hash<Perm16>()(RandPerm) != 0);
}

BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(PPerm16_test)
//****************************************************************************//

BOOST_AUTO_TEST_CASE(PPerm16_constructor) {
    const uint8_t FF = 0xff;
    BOOST_TEST(
        PPerm16({4, 5, 0}, {9, 0, 1}) ==
        PPerm16({1, FF, FF, FF, 9, 0, FF, FF, FF, FF, FF, FF, FF, FF, FF, FF}));
    BOOST_TEST(
        PPerm16({4, 5, 0, 8}, {9, 0, 1, 2}) ==
        PPerm16({1, FF, FF, FF, 9, 0, FF, FF, 2, FF, FF, FF, FF, FF, FF, FF}));
}

BOOST_AUTO_TEST_CASE(PPerm16_hash) {
    BOOST_TEST(std::hash<PPerm16>()(PPerm16::one()) != 0);
    BOOST_TEST(std::hash<PPerm16>()(PPerm16({4, 5, 0}, {9, 0, 1})) != 0);
}

BOOST_FIXTURE_TEST_CASE(PPerm16_left_one, Fix) {
    BOOST_TEST(PPerm16({}).left_one() == PPerm16::one());
    BOOST_TEST(PPerm16({FF, FF, FF, 4}).left_one() ==
               PPerm16({FF, FF, FF, FF}));
    BOOST_TEST(PPerm16({FF, 4, FF, FF}).left_one() ==
               PPerm16({FF, FF, FF, FF}));
    for (auto pp : PPlist) {
        BOOST_TEST(pp.left_one() * pp == pp);
    }
}

BOOST_FIXTURE_TEST_CASE(PPerm16_right_one, Fix) {
    BOOST_TEST(PPerm16({}).right_one() == PPerm16::one());
    BOOST_TEST(PPerm16({FF, FF, FF, 4}).right_one() == PPerm16({FF, FF, FF}));
    BOOST_TEST(PPerm16({FF, 4, FF, FF}).right_one() ==
               PPerm16({FF, 1, FF, FF}));
    for (auto pp : PPlist) {
        BOOST_TEST(pp * pp.right_one() == pp);
    }
}

//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(PPerm16_inverse_ref, Fix) {
    for (epu8 mask : PPmasks) {
        for (Perm16 p : Plist) {
            PPerm16 pp(p.v | mask);
            PPerm16 pi = pp.inverse_ref();
            BOOST_TEST(pp * pi * pp == pp);
            BOOST_TEST(pi * pp * pi == pi);
            BOOST_TEST(pp.inverse_ref().inverse_ref() == pp);
        }
    }
}
TEST_AGREES(PPerm16, inverse_ref, inverse_find, PPlist);

BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(Perm16_mathematical_methods)
//****************************************************************************//

BOOST_FIXTURE_TEST_CASE(Perm16_fix_points_mask, Fix) {
    EPU8_EQUAL(PTransf16::one().fix_points_mask(), Epu8(FF));
    EPU8_EQUAL(Perm16::one().fix_points_mask(), Epu8(FF));
    EPU8_EQUAL(PPa.fix_points_mask(), Epu8({0, 0, 0, 0, 0}, FF));
    EPU8_EQUAL(PPb.fix_points_mask(),
               (epu8{0, 0, 0, 0, 0, FF, 0, FF, FF, FF, FF, FF, FF, 0, FF, 0}));
    EPU8_EQUAL(RandPerm.fix_points_mask(), Epu8({0, FF}, 0));

    EPU8_EQUAL(Perm16::one().fix_points_mask(false), Epu8(FF));
    EPU8_EQUAL(PPa.fix_points_mask(false), Epu8({0, 0, 0, 0, 0}, FF));
    EPU8_EQUAL(PPb.fix_points_mask(false),
               (epu8{0, 0, 0, 0, 0, FF, 0, FF, FF, FF, FF, FF, FF, 0, FF, 0}));
    EPU8_EQUAL(RandPerm.fix_points_mask(false), Epu8({0, FF}, 0));

    EPU8_EQUAL(Perm16::one().fix_points_mask(true), Epu8(0));
    EPU8_EQUAL(PPa.fix_points_mask(true), Epu8({FF, FF, FF, FF, FF}, 0));
    EPU8_EQUAL(PPb.fix_points_mask(true),
               (epu8{FF, FF, FF, FF, FF, 0, FF, 0, 0, 0, 0, 0, 0, FF, 0, FF}));
    EPU8_EQUAL(RandPerm.fix_points_mask(true), Epu8({FF, 0}, FF));
}

BOOST_FIXTURE_TEST_CASE(Perm16_smallest_fix_point, Fix) {
    BOOST_TEST(Perm16::one().smallest_fix_point() == 0);
    BOOST_TEST(PPa.smallest_fix_point() == 5);
    BOOST_TEST(PPb.smallest_fix_point() == 5);
    BOOST_TEST(RandPerm.smallest_fix_point() == 1);
}
BOOST_FIXTURE_TEST_CASE(Perm16_smallest_moved_point, Fix) {
    BOOST_TEST(Perm16::one().smallest_moved_point() == FF);
    BOOST_TEST(PPa.smallest_moved_point() == 0);
    BOOST_TEST(PPb.smallest_moved_point() == 0);
    BOOST_TEST(RandPerm.smallest_moved_point() == 0);
    BOOST_TEST(Perm16({0, 1, 3, 2}).smallest_moved_point() == 2);
}

BOOST_FIXTURE_TEST_CASE(Perm16_largest_fix_point, Fix) {
    BOOST_TEST(Perm16::one().largest_fix_point() == 15);
    BOOST_TEST(PPa.largest_fix_point() == 15);
    BOOST_TEST(PPb.largest_fix_point() == 14);
    BOOST_TEST(RandPerm.largest_fix_point() == 1);
}
BOOST_FIXTURE_TEST_CASE(Perm16_nb_fix_points, Fix) {
    BOOST_TEST(Perm16::one().nb_fix_points() == 16);
    BOOST_TEST(PPa.nb_fix_points() == 11);
    BOOST_TEST(PPb.nb_fix_points() == 8);
    BOOST_TEST(RandPerm.nb_fix_points() == 1);
    BOOST_TEST(Perm16({0, 1, 3, 2}).nb_fix_points() == 14);
}

//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(Perm16_inverse_ref, Fix) {
    BOOST_TEST(PPa * PPa.inverse() == Perm16::one());
    BOOST_TEST(PPa.inverse() * PPa == Perm16::one());
    BOOST_TEST(PPb * PPb.inverse() == Perm16::one());
    BOOST_TEST(PPb.inverse() * PPb == Perm16::one());
    BOOST_TEST(RandPerm * RandPerm.inverse() == Perm16::one());
    BOOST_TEST(RandPerm.inverse() * RandPerm == Perm16::one());

    for (Perm16 p : Plist) {
        BOOST_TEST(p * p.inverse() == Perm16::one());
        BOOST_TEST(p.inverse() * p == Perm16::one());
    }
}
TEST_AGREES(Perm16, inverse_ref, inverse_arr, Plist);
TEST_AGREES(Perm16, inverse_ref, inverse_sort, Plist);
TEST_AGREES(Perm16, inverse_ref, inverse_find, Plist);
TEST_AGREES(Perm16, inverse_ref, inverse_pow, Plist);
TEST_AGREES(Perm16, inverse_ref, inverse_cycl, Plist);
TEST_AGREES(Perm16, inverse_ref, inverse, Plist);

//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(Perm16_lehmer_ref, Fix) {
    EPU8_EQUAL(Perm16::one().lehmer(), zero);
    EPU8_EQUAL(PPa.lehmer(),
               (epu8{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EPU8_EQUAL(PPb.lehmer(),
               (epu8{1, 1, 1, 3, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0}));
}
TEST_EPU8_AGREES(Perm16, lehmer_ref, lehmer_arr, Plist);
TEST_EPU8_AGREES(Perm16, lehmer_ref, lehmer, Plist);

//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(Perm16_length_ref, Fix) {
    BOOST_TEST(Perm16::one().length() == 0);
    BOOST_TEST(PPa.length() == 4);
    BOOST_TEST(PPb.length() == 10);
}
TEST_AGREES(Perm16, length_ref, length_arr, Plist);
TEST_AGREES(Perm16, length_ref, length, Plist);

//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(Perm16_nb_descents_ref, Fix) {
    BOOST_TEST(Perm16::one().nb_descents_ref() == 0);
    BOOST_TEST(PPa.nb_descents_ref() == 1);
    BOOST_TEST(PPb.nb_descents_ref() == 4);
    BOOST_TEST(Perm16::one().nb_descents() == 0);
}
TEST_AGREES(Perm16, nb_descents_ref, nb_descents, Plist);

//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(Perm16_nb_cycles_ref, Fix) {
    BOOST_TEST(Perm16::one().nb_cycles_ref() == 16);
    BOOST_TEST(PPa.nb_cycles_ref() == 12);
    BOOST_TEST(PPb.nb_cycles_ref() == 10);
}
TEST_AGREES(Perm16, nb_cycles_ref, nb_cycles, Plist);

//****************************************************************************//
BOOST_FIXTURE_TEST_CASE(Perm16_left_weak_leq_ref, Fix) {
    BOOST_TEST(Perm16::one().left_weak_leq_ref(Perm16::one()));
    BOOST_TEST(Perm16::one().left_weak_leq_ref(PPa));
    BOOST_TEST(Perm16::one().left_weak_leq_ref(PPb));
    BOOST_TEST(PPa.left_weak_leq_ref(PPa));
    BOOST_TEST(PPb.left_weak_leq_ref(PPb));
}

BOOST_FIXTURE_TEST_CASE(Perm16_left_weak_leq, Fix) {
    for (auto u : PlistSmall) {
        for (auto v : PlistSmall) {
            BOOST_TEST(u.left_weak_leq(v) == u.left_weak_leq_ref(v));
            BOOST_TEST(u.left_weak_leq_length(v) == u.left_weak_leq_ref(v));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//
