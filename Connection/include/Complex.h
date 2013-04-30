#ifndef DDG_COMPLEX_H
#define DDG_COMPLEX_H

#include <iosfwd>

namespace DDG
{
   class Complex
   {
      public:
         Complex( double a=0., double b=0. );
         // constructs number a+bi

         void operator+=( const Complex& z );
         // add z

         void operator-=( const Complex& z );
         // subtract z

         void operator*=( const Complex& z );
         // Complex multiply by z

         void operator*=( double r );
         // scalar multiply by r

         void operator/=( double r );
         // scalar divide by r

         void operator/=( const Complex& z );
         // complex divide by r

         Complex operator-( void ) const;
         // returns the additive inverse

         Complex conj( void ) const;
         // returns Complex conjugate

         Complex inv( void ) const;
         // returns inverse

         double arg( void ) const;
         // returns argument

         double norm( void ) const;
         // returns norm

         double norm2( void ) const;
         // returns norm squared

         Complex unit( void ) const;
         // returns complex number with unit norm and same modulus

         Complex exponential( void ) const;
         // complex exponentiation

         double re;
         // real part

         double im;
         // imaginary part
   };

   Complex operator+( const Complex& z1, const Complex& z2 );
   // binary addition

   Complex operator-( const Complex& z1, const Complex& z2 );
   // binary subtraction
   
   Complex operator*( const Complex& z1, const Complex& z2 );
   // binary Complex multiplication

   Complex operator*( const Complex& z, double r );
   // right scalar multiplication

   Complex operator*( double r, const Complex& z );
   // left scalar multiplication
   
   Complex operator/( const Complex& z, double r );
   // scalar division

   Complex operator/( const Complex& z1, const Complex& z2 );
   // complex division

   double dot( const Complex& z1, const Complex& z2 );
   // inner product

   double cross( const Complex& z1, const Complex& z2 );
   // cross product

   std::ostream& operator<<( std::ostream& os, const Complex& o );
   // prints components
}

#endif
