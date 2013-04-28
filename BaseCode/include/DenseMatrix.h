// -----------------------------------------------------------------------------
// libDDG -- DenseMatrix.h
// -----------------------------------------------------------------------------
//
// DenseMatrix represents an m by n (real or complex) matrix where every
// entry -- including zero-valued entries -- is stored explicitly.  This
// class is most commonly used to represent dense vectors in sparse linear
// systems (i.e., the right hand side and the solution vector).
//
// A real or complex matrix is allocated via
//
//    DenseMatrix A( m, n );
//    DenseMatrix A( m, n, entryComplex );
//
// Matrix elements are then accessed using parenthesis, e.g.,
//
//    A(i,j) = 1;
//    A(i,j) += 2;
//    a = A(i,j);
//
// etc.
//
// DenseMatrix is interoperable with the SuiteSparse numerical linear algebra
// library.  In particular, dereferencing a DenseMatrix returns a cholmod_dense*
// which can be used by routines in SuiteSparse.  For basic operations, however,
// you should not need to access this pointer explicitly -- see the solve()
// method in SparseMatrix.h.
// 

#ifndef DDG_DENSEMATRIX_H
#define DDG_DENSEMATRIX_H

#include <cholmod.h>
#include "Types.h"

#include <vector>

namespace DDG
{
   enum NormType
   {
      lInfinity,
      lOne,
      lTwo
   };

   template <class T>
   class DenseMatrix
   {
      public:
         DenseMatrix( int m = 0, int n = 1 );
         // initialize an mxn matrix (specifying just m yields a column vector)
         
         DenseMatrix( const DenseMatrix<T>& A );
         // copy constructor

         const DenseMatrix<T>& operator=( const DenseMatrix<T>& B );
         // copies B
         
         ~DenseMatrix( void );
         // destructor

         SparseMatrix<T> sparse( void );
         // converts to a sparse matrix

         int nRows( void ) const;
         // returns the number of rows

         int nColumns( void ) const;
         // returns the number of columns

         int length( void ) const;
         // returns the size of the largest dimension

         void zero( const T& val = 0. );
         // sets all elements to val

         double norm( NormType type = lInfinity ) const;
         // returns the maximum magnitude of any entry

         T& operator()( int row, int col );
         T  operator()( int row, int col ) const;
         // access the specified element of the matrix (uses 0-based indexing)

         T& operator()( int index );
         T  operator()( int index ) const;
         // access the specified element of a vector (uses 0-based indexing)
         
         DenseMatrix<T> transpose( void ) const;
         // returns the transpose of this matrix

         DenseMatrix<T> operator*( const DenseMatrix<T>& B ) const;
         // returns product of this matrix with B

         void operator*=( const T& c );
         // multiplies this matrix by the scalar c

         void operator/=( const T& c );
         // divides this matrix by the scalar c

         DenseMatrix operator+( const DenseMatrix& B ) const;
         // returns sum of this matrix with B

         void operator+=( const DenseMatrix& B );
         // adds B to this matrix

         DenseMatrix operator-( const DenseMatrix& B ) const;
         // returns difference of this matrix with B

         void operator-=( const DenseMatrix& B );
         // subtracts B from this matrix

         DenseMatrix<T> operator-( void ) const;
         // returns additive inverse of this matrix

         cholmod_dense* to_cholmod( void );
         // returns pointer to copy of matrix in CHOLMOD format

         const DenseMatrix<T>& operator=( cholmod_dense* B );
         // copies a cholmod_dense* into a DenseMatrix;
         // takes responsibility for deallocating B

         void normalize( void );
         // divides by Frobenius norm

         T sum( void ) const;
         // returns the sum of all entries

         void removeMean( void );
         // removes the mean

         void randomize( void );
         // replaces entries with uniformly distributed random real numbers in the interval [-1,1]

      protected:
         int m, n;
         std::vector<T> data;
         cholmod_dense* cData;
   };

   template <class T>
   DenseMatrix<T> operator*( const DenseMatrix<T>& A, const T& c );
   // right scalar multiplication

   template <class T>
   DenseMatrix<T> operator*( const T& c, const DenseMatrix<T>& A );
   // left scalar multiplication

   template <class T>
   DenseMatrix<T> operator/( const DenseMatrix<T>& A, const T& c );
   // scalar division

   template <class T>
   T dot( const DenseMatrix<T>& x, const DenseMatrix<T>& y );
   // returns Euclidean inner product of x and y

   template <class T>
   std::ostream& operator << (std::ostream& os, const DenseMatrix<T>& o);
   // prints entries

   template <class T>
   T inner( const DenseMatrix<T>& x,
            const DenseMatrix<T>& y );
   // standard inner product

   template <class T>
   T inner( const DenseMatrix<T>& x,
            const DenseMatrix<T>& B,
            const DenseMatrix<T>& y );
   // inner product with respect to a diagonal inner
   // product B represented as a dense vector
}

#include "DenseMatrix.inl"

#endif

