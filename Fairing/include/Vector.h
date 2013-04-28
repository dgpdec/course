// -----------------------------------------------------------------------------
// libDDG -- Vector.h
// -----------------------------------------------------------------------------
//
// Vector represents an element of Euclidean 3-space, along with all the usual
// vectors space operations (addition, multiplication by scalars, etc.).  The
// inner product (i.e., scalar or dot product) is expressed using the global
// method dot():
//
//    Vector u, v;
//    double cosTheta = dot( u, v );
//
// and the cross product is expressed using the global method cross():
//
//    Vector u, v, w;
//    w = cross( u, v );
//
// Individual components can be accessed in two ways: either directly via the
// members x, y, and z:
//
//    Vector v;
//    cout << v.x << endl;
//    cout << v.y << endl;
//    cout << v.z << endl;
//
// or by index:
//
//    Vector v;
//    for( int i = 0; i < 3; i++ )
//    {
//       cout << v[i] << endl;
//    }
//

#ifndef DDG_VECTOR_H
#define DDG_VECTOR_H

#include <iostream>

namespace DDG
{
   class Vector
   {
      public:
         Vector();
         // initializes all components to zero

         Vector( double x, double y, double z);
         // initializes with specified components

         Vector( const Vector& v );
         // initializes from existing vector

         double& operator[] ( const int& index );
         // returns reference to the specified component (0-based indexing: x, y, z)

         const double& operator[] ( const int& index ) const;
         // returns const reference to the specified component (0-based indexing: x, y, z)

         Vector operator+( const Vector& v ) const;
         // addition

         Vector operator-( const Vector& v ) const;
         // subtraction

         Vector operator-( void ) const;
         // negation

         Vector operator*( const double& c ) const;
         // right scalar multiplication

         Vector operator/( const double& c ) const;
         // scalar division

         void operator+=( const Vector& v );
         // addition / assignment

         void operator-=( const Vector& v );
         // subtraction / assignment

         void operator*=( const double& c );
         // scalar multiplication / assignment

         void operator/=( const double& c );
         // scalar division / assignment

         double norm( void ) const;
         // returns Euclidean length

         double norm2( void ) const;
         // returns Euclidean length squared

         Vector unit( void ) const;
         // returns unit vector

         void normalize( void );
         // divides by Euclidean length

         Vector abs( void ) const;
         // returns vector containing magnitude of each component
   
         double x, y, z;
         // components
   };
   
   Vector operator* ( const double& c, const Vector& v );
   // left scalar multiplication

   double dot( const Vector& u, const Vector& v );
   // dot product (a.k.a. inner or scalar product)

   Vector cross( const Vector& u, const Vector& v );
   // cross product

   std::ostream& operator << (std::ostream& os, const Vector& o);
   // prints components
}

#endif

