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

// We check that multiple inclusion of HPCombi works

#include "hpcombi.hpp"

#define PPCAT_NX(A, B) A##B
#define PPCAT(A, B) PPCAT_NX(A, B)

int PPCAT(foo, CONST_TO_BE_CHANGED)() {
    HPCombi::Perm16 res = HPCombi::Perm16::one();
    res = res * res;
    res = res * res;
    res = res * res;
    HPCombi::epu8 rnd = HPCombi::random_epu8(255);
    rnd = rnd + rnd;
    HPCombi::BMat8 resb = HPCombi::BMat8::one();
    resb = resb * resb;
    HPCombi::BMat8 rndb = HPCombi::BMat8::random();
    rndb = rndb * rndb;
    return CONST_TO_BE_CHANGED;
}
