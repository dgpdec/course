// -----------------------------------------------------------------------------
// libDDG -- LinearPolynomial.h
// -----------------------------------------------------------------------------
//
// LinearPolynomial represents an affine function of the form
//
//   f(x1,x2,...,xn) = c1 x1 + c2 x2 + ... + cn xn + d
//
// where the xi are real-valued variables with real coefficients ci, and d is
// a real constant.  The variables and their coefficients are represented using
// instances of the Variable class.  LinearPolynomial implements all the usual
// algebraic operations on affine functions, as well as type conversions from
// more elementary types (scalars, single variables, etc.).
//
// Importantly, variables used in a LinearPolynomial should *not* be deallocated
// while the polynomial is still in use -- LinearPolynomial stores only a
// reference to these variables so that the solution to a linear system can be
// automatically copied back into the variables.
// 

#ifndef DDG_LINEARPOLYNOMIAL_H
#define DDG_LINEARPOLYNOMIAL_H

#include <map>
#include <iosfwd>
#include "Variable.h"

namespace DDG
{
   class LinearPolynomial
   {
      public:
         LinearPolynomial( void );
         // constructs the zero function

         LinearPolynomial( double c );
         // constructs the constant function with value c

         LinearPolynomial( Variable& v );
         // constructs a function with a single variable v

         const LinearPolynomial& operator=( double c );
         // assigns the constant function with value c

         const LinearPolynomial& operator=( Variable& v );
         // assigns a function with a single variable v

         void operator+=( double c );
         void operator-=( double c );
         void operator*=( double c );
         void operator/=( double c );
         // adds, subtract, multiplies, or divides by a constant

         void operator+=( Variable& v );
         void operator-=( Variable& v );
         // increments or decrements by a single variable v

         void operator+=( const LinearPolynomial& p );
         void operator-=( const LinearPolynomial& p );
         // increments or decrements by an affine function

         LinearPolynomial operator-( void ) const;
         // returns the additive inverse (i.e., negation)

         double evaluate( void ) const;
         // evaluates the function using the current values of its variables

         std::map<Variable*,double> linearTerms;
         // list of linear terms

         double constantTerm;
         // constant term
   };

   LinearPolynomial operator+( double c, Variable& v ); // sum
   LinearPolynomial operator+( Variable& v, double c ); // sum
   LinearPolynomial operator-( double c, Variable& v ); // difference
   LinearPolynomial operator-( Variable& v, double c ); // difference
   LinearPolynomial operator*( double c, Variable& v ); // product
   LinearPolynomial operator*( Variable& v, double c ); // product
   LinearPolynomial operator/( Variable& v, double c ); // quotient
   // algebraic operations between single variables and constants

   LinearPolynomial operator+( Variable& v1, Variable& v2 ); // sum
   LinearPolynomial operator-( Variable& v1, Variable& v2 ); // difference
   // algebraic operations between pairs of variables

   LinearPolynomial operator+( const LinearPolynomial& p, double c ); // sum
   LinearPolynomial operator+( double c, const LinearPolynomial& p ); // sum
   LinearPolynomial operator-( const LinearPolynomial& p, double c ); // difference
   LinearPolynomial operator-( double c, const LinearPolynomial& p ); // difference
   LinearPolynomial operator*( const LinearPolynomial& p, double c ); // product
   LinearPolynomial operator*( double c, const LinearPolynomial& p ); // product
   LinearPolynomial operator/( const LinearPolynomial& p, double c ); // quotient
   // algebraic operations between polynomials and constants

   LinearPolynomial operator+( const LinearPolynomial& p, Variable& v ); // sum
   LinearPolynomial operator+( Variable& v, const LinearPolynomial& p ); // sum
   LinearPolynomial operator-( const LinearPolynomial& p, Variable& v ); // difference
   LinearPolynomial operator-( Variable& v, const LinearPolynomial& p ); // difference
   // algebraic operations between polynomials and single variables

   LinearPolynomial operator+( const LinearPolynomial& p, const LinearPolynomial& q ); // sum
   LinearPolynomial operator-( const LinearPolynomial& p, const LinearPolynomial& q ); // difference
   // algebraic operations between pairs of polynomials

   std::ostream& operator<<( std::ostream& os, const LinearPolynomial& p );
   // prints the symbolic representation of a polynomial (all variables must be named)
}

#endif
