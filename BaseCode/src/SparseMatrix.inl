#include <algorithm>
#include <cassert>
#include <iostream>
#include <cmath>
#include <complex>
using namespace std;

#include <SuiteSparseQR.hpp>
#include <umfpack.h>

#include "Real.h"
#include "Complex.h"
#include "SparseMatrix.h"
#include "DenseMatrix.h"
#include "LinearContext.h"
#include "Utility.h"

namespace DDG
{
   extern LinearContext context;

   const int maxEigIter = 20;
   // number of iterations used to solve eigenvalue problems

   template <class T>
   SparseMatrix<T> :: SparseMatrix( int m_, int n_ )
   // initialize an mxn matrix
   : m( m_ ),
     n( n_ ),
     cData( NULL )
   {}

   template <class T>
   SparseMatrix<T> :: SparseMatrix( const SparseMatrix<T>& B )
   // copy constructor
   : cData( NULL )
   {
      *this = B;
   }

   template <class T>
   SparseMatrix<T> :: ~SparseMatrix( void )
   // destructor
   {
      if( cData )
      {
         cholmod_l_free_sparse( &cData, context );
      }
   }

   template <class T>
   const SparseMatrix<T>& SparseMatrix<T> :: operator=( const SparseMatrix<T>& B )
   // copies B
   {
      if( cData )
      {
         cholmod_l_free_sparse( &cData, context );
         cData = NULL;
      }

      m = B.m;
      n = B.n;
      data = B.data;

      return *this;
   }

   template <class T>
   SparseMatrix<T> SparseMatrix<T> :: transpose( void ) const
   {
      SparseMatrix<T> AT( n, m );

      for( const_iterator e  = begin();
                          e != end();
                          e++ )
      {
         int i = e->first.second;
         int j = e->first.first;
         T Aij = e->second;

         AT(j,i) = Aij.conj();
      }

      return AT;
   }

   template <class T>
   SparseMatrix<T> SparseMatrix<T> :: operator*( const SparseMatrix<T>& B ) const
   // returns product of this matrix with sparse B
   {
      const SparseMatrix<T>& A( *this );

      // make sure matrix dimensions agree
      assert( A.nColumns() == B.nRows() );

      // collect nonzeros in each row
      vector< vector< int > > Bcol( B.nRows() );
      vector< vector<  T  > > Bval( B.nRows() );
      for( const_iterator e  = B.begin();
                          e != B.end();
                          e ++ )
      {
         int row = e->first.second;
         int col = e->first.first;
         T val = e->second;

         Bcol[ row ].push_back( col );
         Bval[ row ].push_back( val );
      }

      // multiply C = A*B
      SparseMatrix<T> C( A.nRows(), B.nColumns() );
      for( const_iterator e  = begin();
                          e != end();
                          e ++ )
      {
         int i = e->first.second;
         int j = e->first.first;

         for( size_t n = 0; n < Bcol[j].size(); n++ )
         {
            int k = Bcol[j][n];

            C( i, k ) += e->second * Bval[j][n];
         }
      }

      return C;
   }

   template <class T>
   DenseMatrix<T> SparseMatrix<T> :: operator*( const DenseMatrix<T>& B ) const
   // returns product of this matrix with dense B
   {
      const SparseMatrix<T>& A( *this );

      // make sure matrix dimensions agree
      assert( A.nColumns() == B.nRows() );

      // multiply C = A*B
      DenseMatrix<T> C( A.nRows(), B.nColumns() );
      for( const_iterator e  = begin();
                          e != end();
                          e ++ )
      {
         int i = e->first.second;
         int j = e->first.first;

         for( int k = 0; k < B.nColumns(); k++ )
         {
            C( i, k ) += e->second * B( j, k );
         }
      }

      return C;
   }

   template <class T>
   void SparseMatrix<T> :: operator*=( const T& c )
   {
      for( iterator e  = begin();
                    e != end();
                    e++ )
      {
         e->second *= c;
      }
   }

   template <class T>
   void SparseMatrix<T> :: operator/=( const T& c )
   {
      for( iterator e  = begin();
                    e != end();
                    e++ )
      {
         e->second /= c;
      }
   }

   template <class T>
   void SparseMatrix<T> :: operator+=( const SparseMatrix<T>& B )
   // adds B to this matrix
   {
      SparseMatrix<T>& A( *this );

      // make sure matrix dimensions agree
      assert( A.nRows() == B.nRows() );
      assert( A.nColumns() == B.nColumns() );

      for( const_iterator e  = B.begin();
                          e != B.end();
                          e++ )
      {
         int i = e->first.second;
         int j = e->first.first;
         const T& Bij( e->second );

         A( i, j ) += Bij;
      }
   }

   template <class T>
   void SparseMatrix<T> :: operator-=( const SparseMatrix<T>& B )
   // subtracts B from this matrix
   {
      SparseMatrix<T>& A( *this );

      // make sure matrix dimensions agree
      assert( A.nRows() == B.nRows() );
      assert( A.nColumns() == B.nColumns() );

      for( const_iterator e  = B.begin();
                          e != B.end();
                          e++ )
      {
         int i = e->first.second;
         int j = e->first.first;
         const T& Bij( e->second );

         A( i, j ) -= Bij;
      }
   }

   template <class T>
   SparseMatrix<T> SparseMatrix<T> :: operator+( const SparseMatrix<T>& B ) const
   // returns sum of this matrix with B
   {
      SparseMatrix<T> C( nRows(), nColumns() );

      C += *this;
      C += B;

      return C;
   }

   template <class T>
   SparseMatrix<T> SparseMatrix<T> :: operator-( const SparseMatrix<T>& B ) const
   // returns sum of this matrix with B
   {
      SparseMatrix<T> C( nRows(), nColumns() );

      C += *this;
      C -= B;

      return C;
   }

   template <class T>
   SparseMatrix<T> operator*( const T& c, const SparseMatrix<T>& A )
   {
      SparseMatrix<T> cA = A;

      for( typename SparseMatrix<T>::iterator e  = cA.begin();
                                              e != cA.end();
                                              e++ )
      {
         e->second = c * e->second;
      }

      return cA;
   }

   template <class T>
   SparseMatrix<T> operator*( const SparseMatrix<T>& A, const T& c )
   {
      SparseMatrix<T> Ac = A;
      
      Ac *= c;

      return Ac;
   }

   template <class T>
   SparseMatrix<T> operator/( const SparseMatrix<T>& A, T c )
   {
      SparseMatrix<T> Ac = A;

      Ac /= c;

      return Ac;
   }

   template <class T>
   void SparseMatrix<T> :: resize( int m_, int n_ )
   {
      m = m_;
      n = n_;

      data.clear();
   }

   template <class T>
   int SparseMatrix<T> :: nRows( void ) const
   // returns the number of rows
   {
      return m;
   }

   template <class T>
   int SparseMatrix<T> :: nColumns( void ) const
   // returns the number of columns
   {
      return n;
   }

   template <class T>
   int SparseMatrix<T> :: length( void ) const
   // returns the size of the largest dimension
   {
      return max( m, n );
   }

   template <class T>
   void SparseMatrix<T> :: zero( const T& val )
   // sets all nonzero elements val
   {
      for( iterator i = begin(); i != end(); i++ )
      {
         i->second = val;
      }
   }

   template <class T>
   SparseMatrix<T> SparseMatrix<T> :: inverse( void ) const
   // returns inverse -- for diagonal matrices only
   {
      assert( m == n ); // matrix must be square

      const SparseMatrix<T>& A( *this );
      SparseMatrix<T> Ainv( m, m );

      for( const_iterator e = begin(); e != end(); e++ )
      {
         int r = e->first.second;
         int c = e->first.first;

         assert( r == c ); // matrix must be diagonal

         Ainv( r, c ) = A( r, c ).inv();
      }
      
      return Ainv;
   }

   template <class T>
   SparseMatrix<T> SparseMatrix<T> :: identity( int N )
   {
      SparseMatrix<T> I( N, N );

      for( int i = 0; i < N; i++ )
      {
         I( i, i ) = 1.;
      }

      return I;
   }

   template <class T>
   DenseMatrix<T> SparseMatrix<T> :: full( void ) const
   // converts to a dense matrix
   {
      const int maxSize = 1048576;
      if( m*n > maxSize )
      {
         cerr << "Error: refusing to convert sparse to dense (too big!)" << "\n";
         exit( 1 );
      }

      const SparseMatrix<T>& A( *this );
      DenseMatrix<T> B( m, n );

      for( int i = 0; i < m; i++ )
      for( int j = 0; j < m; j++ )
      {
         B( i, j ) = A( i, j );
      }

      return B;
   }

   template <class T>
   cholmod_sparse* SparseMatrix<T> :: to_cholmod( void )
   {
      if( cData )
      {
         cholmod_l_free_sparse( &cData, context );
         cData = NULL;
      }

      allocateSparse();

      // build compressed matrix (note that EntryMap stores entries in column-major order)
       double* pr =  (double*) cData->x;
      UF_long* ir = (UF_long*) cData->i;
      UF_long* jc = (UF_long*) cData->p;
      int i = 0;
      int j = -1;
      for( const_iterator e  = begin();
                          e != end();
                          e ++ )
      {
         int c = e->first.first;
         if( c != j )
         {
            for( int k = j+1; k <= c; k++ )
            {
               jc[k] = i;
            }
            j = c;
         }

         ir[i] = e->first.second;
         setEntry( e, i, pr );

         i++;
      }
      for( int k = j+1; k <= n; k++ )
      {
         jc[k] = i;
      }

      return cData;
   }

   template <>
   cholmod_sparse* SparseMatrix<Quaternion> :: to_cholmod( void );

   template <class T>
   T& SparseMatrix<T> :: operator()( int row, int col )
   {
      EntryIndex index( col, row );
      const_iterator entry = data.find( index );

      if( entry == end())
      {
         data[ index ] = T( 0. );
      }

      return data[ index ];
   }

   template <class T>
   T SparseMatrix<T> :: operator()( int row, int col ) const
   {
      EntryIndex index( col, row );
      const_iterator entry = data.find( index );

      if( entry == end())
      {
         return T( 0. );
      }

      return entry->second;
   }

   template <class T>
   typename SparseMatrix<T>::iterator SparseMatrix<T> :: begin( void )
   {
      return data.begin();
   }

   template <class T>
   typename SparseMatrix<T>::const_iterator SparseMatrix<T> :: begin( void ) const
   {
      return data.begin();
   }

   template <class T>
   typename SparseMatrix<T>::iterator SparseMatrix<T> :: end( void )
   {
      return data.end();
   }

   template <class T>
   typename SparseMatrix<T>::const_iterator SparseMatrix<T> :: end( void ) const
   {
      return data.end();
   }

   template <class T>
   void SparseMatrix<T> :: shift( double c )
   // adds c times the identity matrix to this matrix
   {
      assert( m == n );
      SparseMatrix<T>& A( *this );
      
      for( int i = 0; i < m; i++ )
      {
         A( i, i ) += c;
      }
   }

   template <class T>
   void solveSymmetric( SparseMatrix<T>& A,
                        DenseMatrix<T>& x,
                        DenseMatrix<T>& b )
   // solves the sparse linear system Ax = b using sparse LU factorization
   {
      int t0 = clock();
      cholmod_sparse* Ac = A.to_cholmod();
      int n = Ac->nrow;
      UF_long* Ap = (UF_long*) Ac->p;
      UF_long* Ai = (UF_long*) Ac->i;
      double*  Ax =  (double*) Ac->x;
      void* Symbolic;
      void* Numeric;

      umfpack_dl_symbolic( n, n, Ap, Ai, Ax, &Symbolic, NULL, NULL );
      umfpack_dl_numeric( Ap, Ai, Ax, Symbolic, &Numeric, NULL, NULL );
      umfpack_dl_solve( UMFPACK_A, Ap, Ai, Ax, (double*) &x(0), (double*) &b(0), Numeric, NULL, NULL );
      umfpack_dl_free_symbolic( &Symbolic );
      umfpack_dl_free_numeric( &Numeric );

      int t1 = clock();
      cout << "[lu] time: " << seconds( t0, t1 ) << "s" << "\n";
      cout << "[lu] max residual: " << residual( A, x, b ) << "\n";
   }

   template <class T>
   void solvePositiveDefinite( SparseMatrix<T>& A,
                                DenseMatrix<T>& x,
                                DenseMatrix<T>& b )
   // solves the positive definite sparse linear system Ax = b using sparse Cholesky factorization
   {
      int t0 = clock();
      cholmod_sparse* Ac = A.to_cholmod();
      Ac->stype = 1;
      cholmod_factor* L = cholmod_l_analyze( Ac, context );
      cholmod_l_factorize( Ac, L, context );
      x = cholmod_l_solve( CHOLMOD_A, L, b.to_cholmod(), context );

      if( L ) cholmod_l_free_factor( &L, context );
      int t1 = clock();

      cout << "[chol] time: " << seconds( t0, t1 ) << "s" << "\n";
      cout << "[chol] max residual: " << residual( A, x, b ) << "\n";
   }

   template <class T>
   void backsolvePositiveDefinite(  SparseFactor<T>& L,
                                     DenseMatrix<T>& x,
                                     DenseMatrix<T>& b )
   // backsolves the prefactored positive definite sparse linear system LL'x = b
   {
      x = cholmod_l_solve( CHOLMOD_A, L.to_cholmod(), b.to_cholmod(), context );
   }

   template <class T>
   void smallestEig( SparseMatrix<T>& A,
                      DenseMatrix<T>& x,
                      bool ignoreConstantVector )
   // solves A x = lambda x for the smallest nonzero eigenvalue lambda
   // A must be symmetric; x is used as an initial guess
   {
      int t0 = clock();
      for( int iter = 0; iter < maxEigIter; iter++ )
      {
         solve( A, x, x );
         if( ignoreConstantVector )
         {
            x.removeMean();
         }
         x.normalize();
      }
      int t1 = clock();

      cout << "[eig] time: " << seconds( t0, t1 ) << "s" << "\n";
      cout << "[eig] max residual: " << residual( A, x ) << "\n";
   }

   template <class T>
   void smallestEig( SparseMatrix<T>& A,
                     SparseMatrix<T>& B,
                      DenseMatrix<T>& x )
   // solves A x = lambda B x for the smallest nonzero generalized eigenvalue lambda
   // A and B must be symmetric; x is used as an initial guess
   {
      // TODO use a symmetric matrix decomposition instead of QR

      int t0 = clock();

      // create vector e that has unit norm w.r.t. B
      int n = A.length();
      DenseMatrix<T> e( n, 1 );
      e.zero( 1. );
      e /= sqrt( dot( e, B*e ));

      for( int iter = 0; iter < maxEigIter; iter++ )
      {
         x = B*x;
         solve( A, x, x );
         x -= dot( x, B*e )*e;
         x.normalize();
      }
      int t1 = clock();

      cout << "[eig] time: " << seconds( t0, t1 ) << "s" << "\n";
      cout << "[eig] max residual: " << residual( A, B, x ) << "\n";
   }

   template <class T>
   void smallestEigPositiveDefinite( SparseMatrix<T>& A,
                                      DenseMatrix<T>& x,
                                      bool ignoreConstantVector )
   // solves A x = lambda x for the smallest nonzero eigenvalue lambda
   // A must be positive (semi-)definite; x is used as an initial guess
   {
      int t0 = clock();
      SparseFactor<T> L;
      L.build( A );

      for( int iter = 0; iter < maxEigIter; iter++ )
      {
         backsolvePositiveDefinite( L, x, x );
         if( ignoreConstantVector )
         {
            x.removeMean();
         }
         x.normalize();
      }
      int t1 = clock();

      cout << "[eig] time: " << seconds( t0, t1 ) << "s" << "\n";
      cout << "[eig] max residual: " << residual( A, x ) << "\n";
   }

   template <class T>
   void smallestEigPositiveDefinite( SparseMatrix<T>& A,
                                     SparseMatrix<T>& B,
                                      DenseMatrix<T>& x )
   // solves A x = lambda x for the smallest nonzero eigenvalue lambda
   // A must be positive (semi-)definite; x is used as an initial guess
   {
      cerr << "Error: A x = lambda B x not properly implemented!" << "\n";
      exit( 1 );

      int t0 = clock();

      SparseFactor<T> L;
      L.build( A );

      // create vector e that has unit norm w.r.t. B
      int n = A.length();
      DenseMatrix<T> e( n, 1 );
      e.zero( 1. );
      e /= sqrt( dot( e, B*e ));

      for( int iter = 0; iter < maxEigIter; iter++ )
      {
         x = B*x;
         backsolvePositiveDefinite( L, x, x );
         x -= dot( x, B*e )*e;
         x.normalize();
      }
      int t1 = clock();

      cout << "[eig] time: " << seconds( t0, t1 ) << "s" << "\n";
      cout << "[eig] max residual: " << residual( A, B, x ) << "\n";
   }

   template <class T>
   void smallestEigPositiveDefinite( SparseMatrix<T>& A,
                                     SparseMatrix<T>& B,
                                      DenseMatrix<T>& E,
                                      DenseMatrix<T>& x )
   // solves A x = lambda (B - EE^T) x for the smallest nonzero eigenvalue lambda
   // A must be positive (semi-)definite, B must be symmetric; EE^T is a low-rank matrix, and
   // x is used as an initial guess
   {
      int iter;
      int t0 = clock();
      DenseMatrix<T> ET = E.transpose();
      SparseFactor<T> L;
      L.build( A );

      for( iter = 0; iter < maxEigIter; iter++ )
      {
         x = B*x - E*(ET*x);
         backsolvePositiveDefinite( L, x, x );
         x.normalize();
      }
      int t1 = clock();

      cout << "[eig] time: " << seconds( t0, t1 ) << "s" << "\n";
      cout << "[eig] max residual: " << residual( A, B, E, x ) << "\n";
   }

   template <class T>
   double residual( const SparseMatrix<T>& A,
                    const  DenseMatrix<T>& x,
                    const  DenseMatrix<T>& b )
   // returns the max residual of the linear problem A x = b relative to the largest entry of the solution
   {
      return ( A*x - b ).norm() / b.norm();
   }

   template <class T>
   double residual( const SparseMatrix<T>& A,
                    const  DenseMatrix<T>& x )
   // returns the max residual of the eigenvalue problem A x = lambda x relative to the largest entry of the solution
   {
      T lambda = rayleighQuotient( A, x );
      return (A*x-lambda*x).norm() / x.norm();
   }

   template <class T>
   double residual( const SparseMatrix<T>& A,
                    const SparseMatrix<T>& B,
                    const  DenseMatrix<T>& x )
   // returns the max residual of the generalized eigenvalue problem A x = lambda x relative to the largest entry of the solution
   {
      T lambda = rayleighQuotient( A, B, x );
      return (A*x-lambda*(B*x)).norm() / x.norm();
   }

   template <class T>
   double residual( const SparseMatrix<T>& A,
                    const SparseMatrix<T>& B,
                    const  DenseMatrix<T>& E,
                    const  DenseMatrix<T>& x )
   // returns the max residual of the generalized eigenvalue problem A x = lambda (B - EE^T) x relative to the largest entry of the solution
   {
      T lambda = rayleighQuotient( A, B, E, x );
      return (A*x-lambda*(B*x-E*(E.transpose()*x))).norm() / x.norm();
   }

   template <class T>
   T rayleighQuotient( const SparseMatrix<T>& A,
                       const  DenseMatrix<T>& x )
   // returns <x,Ax>/<x,x>
   {
      return (x.transpose()*(A*x))(0) * (x.transpose()*x)(0).inv();
   }

   template <class T>
   T rayleighQuotient( const SparseMatrix<T>& A,
                       const SparseMatrix<T>& B,
                       const  DenseMatrix<T>& x )
   // returns <Ax,x>/<Bx,x>
   {
      return (x.transpose()*(A*x))(0) * (x.transpose()*(B*x))(0).inv();
   }

   template <class T>
   T rayleighQuotient( const SparseMatrix<T>& A,
                       const SparseMatrix<T>& B,
                       const  DenseMatrix<T>& E,
                       const  DenseMatrix<T>& x )
   // returns <Ax,x>/<(B-EE^T)x,x>
   {
      return (x.transpose()*(A*x))(0) * (x.transpose()*(B*x-E*(E.transpose()*x)))(0).inv();
   }

   template <class T>
   std::ostream& operator<<( std::ostream& os, const SparseMatrix<T>& o)
   {
      os.precision( 3 );

      for( typename SparseMatrix<T>::const_iterator e  = o.begin();
                                                    e != o.end();
                                                    e ++ )
      {
         int row = e->first.second;
         int col = e->first.first;

         os << "( " << row << ", " << col << " ): " << e->second << "\n";
      }

      return os;
   }

   template <class T>
   SparseFactor<T> :: SparseFactor( void )
   : L( NULL )
   {}

   template <class T>
   SparseFactor<T> :: ~SparseFactor( void )
   {
      if( L )
      {
         cholmod_l_free_factor( &L, context );
      }
   }

   template <class T>
   void SparseFactor<T> :: build( SparseMatrix<T>& A )
   {
      if( L )
      {
         cholmod_l_free_factor( &L, context );
         L = NULL;
      }

      int t0, t1;

      cholmod_sparse* Ac = A.to_cholmod();
      Ac->stype = 1;

      t0 = clock();
      L = cholmod_l_analyze( Ac, context );
      t1 = clock();
      cerr << "analyze: " << seconds(t0,t1) << "s" << endl;

      t0 = clock();
      cholmod_l_factorize( Ac, L, context );
      t1 = clock();
      cerr << "factorize: " << seconds(t0,t1) << "s" << endl;
   }

   template <class T>
   bool SparseFactor<T> :: valid( void ) const
   {
      if( L == NULL )
      {
         return false;
      }
      return true;
   }

   template <class T>
   cholmod_factor* SparseFactor<T> :: to_cholmod( void )
   {
      return L;
   }
}

