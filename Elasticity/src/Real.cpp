#include "Real.h"
#include <cmath>

namespace DDG
{
   Real :: Real( double x )
   // constructs real number with value x
   : value( x )
   {}

   Real :: operator double( void ) const
   // type cast to double
   {
      return value;
   }

   void Real :: operator+=( double x )
   // increment
   {
      value += x;
   }

   void Real :: operator-=( double x )
   // decrement
   {
      value -= x;
   }

   void Real :: operator*=( double x )
   // multiply
   {
      value *= x;
   }

   void Real :: operator/=( double x )
   // divide
   {
      value /= x;
   }

   Real Real :: conj( void ) const
   // simply returns the value (for compatibility w/ complex numbers)
   {
      return value;
   }

   Real Real :: inv( void ) const
   // returns inverse
   {
      return 1. / value;
   }

   double Real :: norm( void ) const
   // returns norm
   {
      return fabs( value );
   }

   double Real :: norm2( void ) const
   // returns norm squared
   {
      return value * value;
   }

   Real Real :: unit( void ) const
   // returns number with unit norm and same sign
   {
      return value / norm();
   }
}

