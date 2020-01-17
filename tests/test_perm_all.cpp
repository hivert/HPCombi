/******************************************************************************/
/*       Copyright (C) 2014 Florent Hivert <Florent.Hivert@lri.fr>,           */
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

#define BOOST_TEST_MODULE perm_generic

#include <iomanip>
#include <vector>

#include <boost/functional.hpp>
#include <boost/mpl/list.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/test/unit_test.hpp>

//____________________________________________________________________________//

template <class VectType> struct IsPermFunctions {
    static bool is_perm(const VectType a) { return a.is_permutation(); };
    static bool is_perm2(const VectType a, size_t i) {
        return a.is_permutation(i);
    };
};

#include "perm16.hpp"
#include "perm_generic.hpp"

//____________________________________________________________________________//

template <class _PermType>
struct Fixture : public IsPermFunctions<typename _PermType::vect> {

    using VectType = typename _PermType::vect;
    using PermType = _PermType;

    Fixture()
        : zero({0}), V01({0, 1}), V10({1, 0}), V11({1, 1}), V1({}, 1),
          PPa({1, 2, 3, 4, 0, 5}), PPb({1, 2, 3, 6, 0, 5}), czero(zero),
          cV01(V01), RandPerm({3, 1, 0, 5, 10, 2, 6, 7, 4, 8, 9}),
          Plist({PPa, PPb, RandPerm}),
          Vlist({zero, V01, V10, V11, V1, PPa, PPb, RandPerm}) {
        BOOST_TEST_MESSAGE("setup fixture");
    }
    ~Fixture() { BOOST_TEST_MESSAGE("teardown fixture"); }

    VectType zero, V01, V10, V11, V1;
    PermType PPa, PPb;
    const VectType czero, cV01;
    const PermType RandPerm;
    const std::vector<PermType> Plist;
    const std::vector<VectType> Vlist;

    static bool less(const VectType a, const VectType b) { return a < b; };
    static bool not_less(const VectType a, const VectType b) {
        return not(a < b);
    };
};

//____________________________________________________________________________//

typedef boost::mpl::list<
    Fixture<HPCombi::Perm16>, Fixture<HPCombi::PermGeneric<12>>,
    Fixture<HPCombi::PermGeneric<16>>, Fixture<HPCombi::PermGeneric<32>>,
    Fixture<HPCombi::PermGeneric<42>>, Fixture<HPCombi::PermGeneric<49>>,
    Fixture<HPCombi::PermGeneric<350, uint32_t>>>
    Fixtures;

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE(VectType_test)
//____________________________________________________________________________//

BOOST_FIXTURE_TEST_CASE_TEMPLATE(sizeof_test, F, Fixtures, F) {
    BOOST_TEST(sizeof(F::zero) == F::VectType::Size() * sizeof(F::zero[0]));
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(equal_test, F, Fixtures, F) {
    BOOST_TEST(F::zero == F::zero);
    BOOST_TEST(F::zero != F::V01);
    for (unsigned i = 0; i < F::Plist.size(); i++)
        for (unsigned j = 0; j < F::Plist.size(); j++)
            if (i == j)
                BOOST_TEST(F::Plist[i] == F::Plist[j]);
            else
                BOOST_TEST(F::Plist[i] != F::Plist[j]);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(operator_bracket_const_test, F, Fixtures, F) {
    BOOST_TEST(F::czero[0] == 0u);
    BOOST_TEST(F::czero[1] == 0u);
    BOOST_TEST(F::czero[15] == 0u);
    BOOST_TEST(F::cV01[0] == 0u);
    BOOST_TEST(F::cV01[1] == 1u);
    BOOST_TEST(F::cV01[2] == 0u);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(operator_bracket_test, F, Fixtures, F) {
    BOOST_TEST(F::zero[0] == 0u);
    BOOST_TEST(F::zero[1] == 0u);
    BOOST_TEST(F::zero[15] == 0u);
    BOOST_TEST(F::V01[0] == 0u);
    BOOST_TEST(F::V01[1] == 1u);
    BOOST_TEST(F::V01[2] == 0u);
    BOOST_TEST(F::PPa[4] == 0u);
    BOOST_TEST(F::PPa[5] == 5u);
    F::zero[0] = 3;
    BOOST_TEST(F::zero[0] == 3u);
    BOOST_TEST(F::zero[1] == 0u);
    BOOST_TEST(F::zero[15] == 0u);
    F::PPa[2] = 0;
    BOOST_TEST(F::PPa[1] == 2u);
    BOOST_TEST(F::PPa[2] == 0u);
    BOOST_TEST(F::PPa[3] == 4u);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(operator_less_test, F, Fixtures, F) {
    for (unsigned i = 0; i < F::Plist.size(); i++)
        for (unsigned j = 0; j < F::Plist.size(); j++)
            if (i < j)
                BOOST_CHECK_PREDICATE(F::less, (F::Plist[i])(F::Plist[j]));
            else
                BOOST_CHECK_PREDICATE(F::not_less, (F::Plist[i])(F::Plist[j]));
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(operator_less_partial_test, F, Fixtures, F) {
    for (auto p : F::Plist)
        for (unsigned k = 0; k < F::PermType::size(); k++)
            BOOST_TEST(p.less_partial(p, k) == 0);
    for (auto p : F::Plist)
        for (auto q : F::Plist)
            BOOST_TEST(p.less_partial(q, 0) == 0);

    BOOST_TEST(F::zero.less_partial(F::V01, 1) == 0);
    BOOST_TEST(F::V01.less_partial(F::zero, 1) == 0);
    BOOST_TEST(F::zero.less_partial(F::V01, 2) < 0);
    BOOST_TEST(F::V01.less_partial(F::zero, 2) > 0);

    BOOST_TEST(F::zero.less_partial(F::V10, 1) < 0);
    BOOST_TEST(F::zero.less_partial(F::V10, 2) < 0);
    BOOST_TEST(F::V10.less_partial(F::zero, 1) > 0);
    BOOST_TEST(F::V10.less_partial(F::zero, 2) > 0);

    BOOST_TEST(F::PPa.less_partial(F::PPb, 1) == 0);
    BOOST_TEST(F::PPa.less_partial(F::PPb, 2) == 0);
    BOOST_TEST(F::PPa.less_partial(F::PPb, 3) == 0);
    BOOST_TEST(F::PPa.less_partial(F::PPb, 4) < 0);
    BOOST_TEST(F::PPa.less_partial(F::PPb, 5) < 0);
    BOOST_TEST(F::PPb.less_partial(F::PPa, 4) > 0);
    BOOST_TEST(F::PPb.less_partial(F::PPa, 5) > 0);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(first_zero_test, F, Fixtures, F) {
    BOOST_TEST(F::zero.first_zero() == 0u);
    BOOST_TEST(F::V01.first_zero() == 0u);
    BOOST_TEST(F::PPa.first_zero() == 4u);
    BOOST_TEST(F::V10.first_zero() == 1u);
    BOOST_TEST(F::V1.first_zero() == F::VectType::Size());
    BOOST_TEST(F::V10.first_zero(1) == F::VectType::Size());
    BOOST_TEST(F::PPa.first_zero(5) == 4u);
    BOOST_TEST(F::PPa.first_zero(3) == F::VectType::Size());
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(last_zero_test, F, Fixtures, F) {
    BOOST_TEST(F::zero.last_zero() == F::VectType::Size() - 1);
    BOOST_TEST(F::V01.last_zero() == F::VectType::Size() - 1);
    BOOST_TEST(F::PPa.last_zero() == 4u);
    BOOST_TEST(F::V1.last_zero() == F::VectType::Size());
    BOOST_TEST(F::V01.last_zero(1) == 0u);
    BOOST_TEST(F::V10.last_zero(1) == F::VectType::Size());
    BOOST_TEST(F::PPa.last_zero(5) == 4u);
    BOOST_TEST(F::PPa.last_zero(3) == F::VectType::Size());
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(first_non_zero_test, F, Fixtures, F) {
    BOOST_TEST(F::zero.first_non_zero() == F::VectType::Size());
    BOOST_TEST(F::V01.first_non_zero() == 1u);
    BOOST_TEST(F::PPa.first_non_zero() == 0u);
    BOOST_TEST(F::V01.first_non_zero() == 1u);
    BOOST_TEST(F::V01.first_non_zero(1) == F::VectType::Size());
    BOOST_TEST(F::PPa.first_non_zero(5) == 0u);
    BOOST_TEST(F::PPa.first_non_zero(3) == 0u);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(last_non_zero_test, F, Fixtures, F) {
    BOOST_TEST(F::zero.last_non_zero() == F::VectType::Size());
    BOOST_TEST(F::V01.last_non_zero() == 1u);
    BOOST_TEST(F::PPa.last_non_zero() == F::VectType::Size() - 1);
    BOOST_TEST(F::V01.last_non_zero() == 1u);
    BOOST_TEST(F::V01.last_non_zero(1) == F::VectType::Size());
    BOOST_TEST(F::PPa.last_non_zero(5) == 3u);
    BOOST_TEST(F::PPa.last_non_zero(3) == 2u);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(permuted_test, F, Fixtures, F) {
    BOOST_TEST(F::zero.permuted(F::zero) == F::zero);
    BOOST_TEST(F::V01.permuted(F::V01) == F::V01);
    BOOST_TEST(F::V10.permuted(F::V10) == typename F::VectType({0, 1}, 1));
    BOOST_TEST(F::V10.permuted(F::V01) == typename F::VectType({1, 0}, 1));
    BOOST_TEST(F::V01.permuted(F::V10) == F::V10);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(operator_insert_test, F, Fixtures, F) {
    std::ostringstream out, out2;
    out << F::zero;
    out2 << "[ 0";
    for (size_t i = 1; i < F::VectType::Size(); i++)
        out2 << ", 0";
    out2 << "]";
    BOOST_TEST(out.str() == out2.str());

    out.str("");
    out2.str("");
    out << F::V01;
    out2 << "[ 0, 1";
    for (size_t i = 2; i < F::VectType::Size(); i++)
        out2 << ", 0";
    out2 << "]";
    BOOST_TEST(out.str() == out2.str());

    out.str("");
    out2.str("");
    out << F::PPa;
    out2 << "[ 1, 2, 3, 4, 0";
    for (size_t i = 5; i < F::VectType::Size(); i++)
        out2 << "," << std::setw(2) << i;
    out2 << "]";
    BOOST_TEST(out.str() == out2.str());
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(is_permutation_test, F, Fixtures, F) {
    BOOST_CHECK_PREDICATE(boost::not1(F::is_perm), (F::zero));
    BOOST_CHECK_PREDICATE(F::is_perm, (F::PPa));
    BOOST_CHECK_PREDICATE(boost::not1(F::is_perm), (F::PPb));
    BOOST_CHECK_PREDICATE(F::is_perm, (F::RandPerm));
    BOOST_CHECK_PREDICATE(
        boost::not1(F::is_perm),
        (typename F::VectType({3, 1, 0, 9, 3, 10, 2, 11, 6, 7, 4, 8})));
    BOOST_CHECK_PREDICATE(F::is_perm2, (F::PPa)(16));
    BOOST_CHECK_PREDICATE(boost::not2(F::is_perm2), (F::RandPerm)(4));
    BOOST_CHECK_PREDICATE(F::is_perm2, (F::PPa)(5));
    BOOST_CHECK_PREDICATE(boost::not2(F::is_perm2), (F::PPa)(4));
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//
//____________________________________________________________________________//

template <class _Perm> struct PermFixture : public IsPermFunctions<_Perm> {
    using PermType = _Perm;
    PermFixture()
        : id(PermType::one()), RandPerm({3, 1, 0, 5, 10, 2, 11, 6, 7, 4, 8, 9}),
          Plist({id, RandPerm}) {
        for (uint64_t i = 0; i < std::min<size_t>(PermType::size(), 30) - 1;
             i++)
            Plist.push_back(PermType::elementary_transposition(i));
        for (uint64_t i = std::max<size_t>(30, PermType::size() - 20);
             i < PermType::size() - 1; i++)
            Plist.push_back(PermType::elementary_transposition(i));
        for (uint64_t i = 0; i < 10; i++)
            Plist.push_back(PermType::random());
        BOOST_TEST_MESSAGE("setup fixture");
    }

    ~PermFixture() { BOOST_TEST_MESSAGE("teardown fixture"); }

    PermType id, s1, s2, s3;
    const PermType RandPerm;
    std::vector<PermType> Plist;
};

//____________________________________________________________________________//

typedef boost::mpl::list<PermFixture<HPCombi::Perm16>,
                         PermFixture<HPCombi::PermGeneric<12>>,
                         PermFixture<HPCombi::PermGeneric<16>>,
                         PermFixture<HPCombi::PermGeneric<32>>,
                         PermFixture<HPCombi::PermGeneric<42>>,
                         PermFixture<HPCombi::PermGeneric<49>>,
                         PermFixture<HPCombi::PermGeneric<350, uint32_t>>>
    PermFixtures;

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE(PermType_test)
//____________________________________________________________________________//

BOOST_FIXTURE_TEST_CASE_TEMPLATE(constructor_is_permutation_test, F,
                                 PermFixtures, F) {
    for (auto x : F::Plist)
        BOOST_CHECK_PREDICATE(F::is_perm, (x));

    // Default constructor doesn't initialize
    // BOOST_CHECK_PREDICATE(F::is_perm, (typename F::PermType()));
    BOOST_CHECK_PREDICATE(F::is_perm, (typename F::PermType({})));
    BOOST_CHECK_PREDICATE(F::is_perm, (typename F::PermType({1, 0})));
    BOOST_CHECK_PREDICATE(F::is_perm, (typename F::PermType({1, 2, 0})));
    BOOST_CHECK_PREDICATE(boost::not1(F::is_perm),
                          (typename F::PermType({1, 2})));
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(std_hash_test, F, PermFixtures, F) {
    for (auto x : F::Plist)
        BOOST_TEST(std::hash<typename F::PermType>()(x) != 0);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(mult_coxeter_test, F, PermFixtures, F) {
    for (uint64_t i = 0; i < F::PermType::Size() - 1; i++) {
        auto si = F::PermType::elementary_transposition(i);
        BOOST_TEST(si != F::id);
        BOOST_TEST(si * si == F::id);
        if (i + 1 < F::PermType::Size() - 1) {
            auto si1 = F::PermType::elementary_transposition(i + 1);
            BOOST_TEST(si * si1 * si == si1 * si * si1);
        }
        for (uint64_t j = i + 2; j < F::PermType::Size() - 1; j++) {
            auto sj = F::PermType::elementary_transposition(j);
            BOOST_TEST(sj * si == si * sj);
        }
    }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(mult_test, F, PermFixtures, F) {
    for (auto x : F::Plist) {
        BOOST_TEST(F::id * x == x);
        BOOST_TEST(x * F::id == x);
    }
    BOOST_TEST(F::RandPerm * F::RandPerm ==
               typename F::PermType({5, 1, 3, 2, 8, 0, 9, 11, 6, 10, 7, 4}));

    for (auto x : F::Plist)
        for (auto y : F::Plist)
            for (auto z : F::Plist)
                BOOST_TEST((x * y) * z == x * (y * z));
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(inverse_test, F, PermFixtures, F) {
    for (auto x : F::Plist) {
        BOOST_TEST(x.inverse() * x == F::id);
        BOOST_TEST(x * x.inverse() == F::id);
        BOOST_TEST(x.inverse().inverse() == x);
    }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(random_test, F, PermFixtures, F) {
    for (int i = 0; i < 100; i++) {
        BOOST_CHECK_PREDICATE(F::is_perm, (F::PermType::random()));
    }
}

BOOST_AUTO_TEST_SUITE_END()
