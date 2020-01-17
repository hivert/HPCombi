//****************************************************************************//
//       Copyright (C) 2018 Florent Hivert <Florent.Hivert@lri.fr>,           //
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

#define BOOST_TEST_MODULE MultIncl_Test

#include <boost/test/unit_test.hpp>

int foo0();  // in test_mincl0.cpp
int foo1();  // in test_mincl1.cpp

BOOST_AUTO_TEST_SUITE(MultIncl)
BOOST_AUTO_TEST_CASE(MultInclFoo0) { BOOST_CHECK_EQUAL(foo0(), 0); }
BOOST_AUTO_TEST_CASE(MultInclFoo1) { BOOST_CHECK_EQUAL(foo1(), 1); }
BOOST_AUTO_TEST_SUITE_END()
