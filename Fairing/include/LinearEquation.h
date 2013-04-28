// -----------------------------------------------------------------------------
// libDDG -- LinearEquation.h
// -----------------------------------------------------------------------------
//
// LinearEquation represents an equation with an arbitrary linear polynomial on
// both the left- and right-hand side.  It is primarily used while building a
// LinearSystem.  For convenience, operator== is overloaded so that the user
// can construct a LinearEquation by writing something that looks much like the
// usual mathematical syntax for a linear equation.  For example,
//
//    LinearEquation eqn = ( x + 2*y == 3*z );
//
// builds the linear equation x + 2y = 3z.
// 

#ifndef DDG_LINEAREQUATION_H
#define DDG_LINEAREQUATION_H

#include "LinearPolynomial.h"

namespace DDG
{
   class LinearEquation
   {
      public:
         LinearPolynomial lhs;
         // left-hand side

         LinearPolynomial rhs;
         // right-hand side
   };

   LinearEquation operator==( const LinearPolynomial& lhs, const LinearPolynomial& rhs );
   // constructs a linear equation with the specified left- and right-hand side
}

#endif

