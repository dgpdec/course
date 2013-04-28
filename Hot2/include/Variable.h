// -----------------------------------------------------------------------------
// libDDG -- Variable.h
// -----------------------------------------------------------------------------
//
// Variable represents a variable that can be used to define a (linear or
// nonlinear) system of equations.  Its main feature is that it can be used
// both as an abstract variable (e.g., when used to define an equation) but can
// also store a definite numerical value.  For instance, suppose we define a
// linear polynomial
//
//    Variable x, y;
//    LinearPolynomial p = x + 2*y;
//
// Now by assigning different numerical values to x and y, we can evaluate the
// polynomial at different points:
//
//    *x = 1;
//    *y = 2;
//    cout << p.evaluate() << endl;
//
//    *x = 3;
//    *y = 4;
//    cout << p.evaluate() << endl;
//
// In general the dereference operator (*) accesses the numerical value.
// Variables can also be named in order to aid with debugging.  For instance,
//
//    Variable x( "x" );
//    Variable y( "y" );
//    Polynomial p = x + 2*y;
//    cout << p << endl;
//
// will print out something like "x+2*y".
//
// The "fixed" flag in a variable refers to whether it is held constant while
// solving a system of equations -- see the documentation for further discussion.
// 

#ifndef DDG_VARIABLE_H
#define DDG_VARIABLE_H

#include <string>

namespace DDG
{
   class Variable
   {
      public:
         Variable( double value = 0., bool fixed = false );
         // initialize a variable which has value zero and is not fixed by default

         Variable( std::string name, double value = 0., bool fixed = false );
         // initialize a named variable which has value zero and is not fixed by default

         double& operator*( void );
         // returns a reference to the numerical value

         const double& operator*( void ) const;
         // returns a const reference to the numerical value

         std::string name;
         // names the variable (for display output)

         double value;
         // numerical value

         bool fixed;
         // true if a variable is held constant while solving a system of equations
   };
}

#endif

