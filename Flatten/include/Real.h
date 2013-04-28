#ifndef DDG_REAL_H
#define DDG_REAL_H

namespace DDG
{
   class Real
   {
      public:
         Real( double x = 0. );
         // constructs real number with value x

         operator double( void ) const;
         // type cast to double

         void operator+=( double x );
         // increment

         void operator-=( double x );
         // decrement

         void operator*=( double x );
         // multiply

         void operator/=( double x );
         // divide

         Real conj( void ) const;
         // simply returns the value (for compatibility w/ complex numbers)

         Real inv( void ) const;
         // returns inverse

         double norm( void ) const;
         // returns norm

         double norm2( void ) const;
         // returns norm squared

         Real unit( void ) const;
         // returns number with unit norm and same sign

      protected:
         double value;
         // value
   };
}

#endif
