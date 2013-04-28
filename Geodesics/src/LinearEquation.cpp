#include "LinearEquation.h"

namespace DDG
{
   LinearEquation operator==( const LinearPolynomial& lhs,
                              const LinearPolynomial& rhs )
   // constructs a linear equation with the specified left- and right-hand side
   {
      LinearEquation eqn;

      eqn.lhs = lhs;
      eqn.rhs = rhs;

      return eqn;
   }
}

