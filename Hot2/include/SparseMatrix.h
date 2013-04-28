// -----------------------------------------------------------------------------
// libDDG -- SparseMatrix.h
// -----------------------------------------------------------------------------
//
// SparseMatrix represents an m by n (real or complex) matrix where only
// nonzero entries are stored explicitly.  This class is most commonly
// used to represent the linear term in sparse linear systems (i.e., the matrix
// part).
//
// A real or complex matrix is allocated via
//
//    SparseMatrix A( m, n );
//    SparseMatrix A( m, n, entryComplex );
//
// Matrix elements are then accessed using parenthesis, e.g.,
//
//    A(i,j) = 1;
//    A(i,j) += 2;
//    a = A(i,j);
//
// etc.
//
// SparseMatrix is interoperable with the SuiteSparse numerical linear algebra
// library.  In particular, dereferencing a SparseMatrix returns a
// cholmod_sparse* which can be used by routines in SuiteSparse.  For basic
// operations, however, you should not need to access this pointer explicitly --
// see the solve() method below.
//
// Internally SparseMatrix stores nonzero entries in a heap data structure; the
// amortized cost of insertion is therefore no worse than the sorting cost of
// putting the matrix in compressed-column order.
// 

#ifndef DDG_SPARSE_MATRIX_H
#define DDG_SPARSE_MATRIX_H

#include <cholmod.h>
#include <vector>
#include <map>
#include "Types.h"

namespace DDG
{
   template<class T>
   class SparseMatrix
   {
      public:
         SparseMatrix( int m = 0, int n = 1 );
         // initialize an mxn matrix

         SparseMatrix( const SparseMatrix<T>& B );
         // copy constructor

         ~SparseMatrix( void );
         // destructor

         const SparseMatrix<T>& operator=( const SparseMatrix<T>& B );
         // copies B

         const SparseMatrix<T>& operator=( cholmod_sparse* B );
         // copies a cholmod_sparse* into a SparseMatrix;
         // takes responsibility for deallocating B

         void resize( int m, int n );
         // clears and resizes to mxn matrix

         SparseMatrix<T> transpose( void ) const;
         // returns the transpose of this matrix
         
         cholmod_sparse* to_cholmod( void );
         // returns pointer to copy of matrix in compressed-column CHOLMOD format

         SparseMatrix<T> operator*( const SparseMatrix<T>& B ) const;
         // returns product of this matrix with sparse B

         DenseMatrix<T> operator*( const DenseMatrix<T>& B ) const;
         // returns product of this matrix with dense B

         void operator*=( const T& c );
         // multiplies this matrix by the scalar c

         void operator/=( const T& c );
         // divides this matrix by the scalar c

         void operator+=( const SparseMatrix<T>& B );
         // adds B to this matrix

         void operator-=( const SparseMatrix<T>& B );
         // subtracts B from this matrix

         SparseMatrix<T> operator+( const SparseMatrix<T>& B ) const;
         // returns sum of this matrix with B

         SparseMatrix<T> operator-( const SparseMatrix<T>& B ) const;
         // returns difference of this matrix with B

         int nRows( void ) const;
         // returns the number of rows

         int nColumns( void ) const;
         // returns the number of columns

         int length( void ) const;
         // returns the size of the largest dimension

         void zero( const T& val );
         // sets all nonzero elements val

         SparseMatrix<T> inverse( void ) const;
         // returns inverse -- for diagonal matrices only
         // (assertion occurs for non-diagonal matrices)
         
         static SparseMatrix<T> identity( int N );
         // returns the N x N identity matrix

         DenseMatrix<T> full( void ) const;
         // converts to a dense matrix

         T& operator()( int row, int col );
         T  operator()( int row, int col ) const;
         // access the specified element (uses 0-based indexing)

         // TODO for legibility, replace w/ type where entries are named "row,
         // TODO col" instead of "first, second" (especially since we adopt the
         // TODO unorthodox convention of storing the column first)
         typedef std::pair<int,int> EntryIndex;
         // convenience type for an entry index; note that we store column THEN
         // row, which makes it easier to build compressed column format

         typedef std::map<EntryIndex,T> EntryMap;
         typedef typename EntryMap::iterator       iterator;
         typedef typename EntryMap::const_iterator const_iterator;
         // convenience types for storing and accessing entries

               iterator begin( void );
         const_iterator begin( void ) const;
               iterator   end( void );
         const_iterator   end( void ) const;
         // return iterators to first and last nonzero entries

         void shift( double c );
         // adds c times the identity matrix to this matrix

      protected:
         int m, n;
         EntryMap data;
         cholmod_sparse* cData;

         void allocateSparse( void );
         void setEntry( const_iterator e, int i, double* pr );
   };

   template <class T>
   SparseMatrix<T> operator*( const SparseMatrix<T>& A, const T& c );
   // right scalar multiplication

   template <class T>
   SparseMatrix<T> operator*( const T& c, const SparseMatrix<T>& A );
   // left scalar multiplication

   template <class T>
   SparseMatrix<T> operator/( const SparseMatrix<T>& A, const T& c );
   // scalar division

   template <class T>
   std::ostream& operator << (std::ostream& os, const SparseMatrix<T>& o);
   // prints entries

   template <class T>
   class SparseFactor
   {
      public:
         SparseFactor( void );
         ~SparseFactor( void );

         void build( SparseMatrix<T>& A );
         // factorizes positive-definite matrix A using CHOLMOD

         bool valid( void ) const;
         // returns true if the factor has been built; false otherwise

         cholmod_factor* to_cholmod( void );
         // returns pointer to underlying cholmod_factor data structure

      protected:
         cholmod_factor *L;
   };

   template <class T>
   void solve( SparseMatrix<T>& A,
                DenseMatrix<T>& x,
                DenseMatrix<T>& b );
   // solves the sparse linear system Ax = b using sparse QR factorization

   template <class T>
   void solveSymmetric( SparseMatrix<T>& A,
                        DenseMatrix<T>& x,
                        DenseMatrix<T>& b );
   // solves the sparse linear system Ax = b using sparse LU factorization

   template <class T>
   void solvePositiveDefinite( SparseMatrix<T>& A,
                                DenseMatrix<T>& x,
                                DenseMatrix<T>& b );
   // solves the positive definite sparse linear system Ax = b using sparse Cholesky factorization

   template <class T>
   void backsolvePositiveDefinite( SparseFactor<T>& L,
                                    DenseMatrix<T>& x,
                                    DenseMatrix<T>& b );
   // backsolves the prefactored positive definite sparse linear system LL'x = b

   template <class T>
   void smallestEig( SparseMatrix<T>& A,
                      DenseMatrix<T>& x,
                      bool ignoreConstantVector = true );
   // solves A x = lambda x for the smallest nonzero eigenvalue lambda
   // A must be symmetric; x is used as an initial guess

   template <class T>
   void smallestEig( SparseMatrix<T>& A,
                     SparseMatrix<T>& B,
                      DenseMatrix<T>& x );
   // solves A x = lambda B x for the smallest nonzero generalized eigenvalue lambda
   // A and B must be symmetric; x is used as an initial guess

   template <class T>
   void smallestEigPositiveDefinite( SparseMatrix<T>& A,
                                      DenseMatrix<T>& x,
                                      bool ignoreConstantVector = true );
   // solves A x = lambda x for the smallest nonzero eigenvalue lambda
   // A must be positive (semi-)definite; x is used as an initial guess

   template <class T>
   void smallestEigPositiveDefinite( SparseMatrix<T>& A,
                                     SparseMatrix<T>& B,
                                      DenseMatrix<T>& x );
   // solves A x = lambda B x for the smallest nonzero eigenvalue lambda
   // A must be positive (semi-)definite, B must be symmetric; x is used as an initial guess

   template <class T>
   void smallestEigPositiveDefinite( SparseMatrix<T>& A,
                                     SparseMatrix<T>& B,
                                      DenseMatrix<T>& E,
                                      DenseMatrix<T>& x );
   // solves A x = lambda (B - EE^T) x for the smallest nonzero eigenvalue lambda
   // A must be positive (semi-)definite, B must be symmetric; EE^T is a low-rank matrix, and
   // x is used as an initial guess

   template <class T>
   double residual( const SparseMatrix<T>& A,
                    const  DenseMatrix<T>& x,
                    const  DenseMatrix<T>& b );
   // returns the max residual of the linear problem A x = b relative to the largest entry of the solution

   template <class T>
   double residual( const SparseMatrix<T>& A,
                    const  DenseMatrix<T>& x );
   // returns the max residual of the eigenvalue problem A x = lambda x relative to the largest entry of the solution

   template <class T>
   double residual( const SparseMatrix<T>& A,
                    const SparseMatrix<T>& B,
                    const  DenseMatrix<T>& x );
   // returns the max residual of the generalized eigenvalue problem A x = lambda B x relative to the largest entry of the solution

   template <class T>
   double residual( const SparseMatrix<T>& A,
                    const SparseMatrix<T>& B,
                    const  DenseMatrix<T>& E,
                    const  DenseMatrix<T>& x );
   // returns the max residual of the generalized eigenvalue problem A x = lambda (B - EE^T) x relative to the largest entry of the solution

   template <class T>
   T rayleighQuotient( const SparseMatrix<T>& A,
                       const  DenseMatrix<T>& x );
   // returns <Ax,x>/<x,x>

   template <class T>
   T rayleighQuotient( const SparseMatrix<T>& A,
                       const SparseMatrix<T>& B,
                       const  DenseMatrix<T>& x );
   // returns <Ax,x>/<Bx,x>

   template <class T>
   T rayleighQuotient( const SparseMatrix<T>& A,
                       const SparseMatrix<T>& B,
                       const  DenseMatrix<T>& E,
                       const  DenseMatrix<T>& x );
   // returns <Ax,x>/<(B-EE^T)x,x>
}

#include "SparseMatrix.inl"

#endif
