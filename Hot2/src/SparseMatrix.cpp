#include "SparseMatrix.h"

namespace DDG
{
   template <>
   const SparseMatrix<Real>& SparseMatrix<Real> :: operator=( cholmod_sparse* B )
   {
      assert( B );
      assert( B->xtype == CHOLMOD_REAL );

      if( cData )
      {
         cholmod_l_free_sparse( &cData, context );
      }
      cData = B;

      m = cData->nrow;
      n = cData->ncol;
      resize( m, n );

      double* pr = (double*) cData->x;
      UF_long* ir = (UF_long*) cData->i;
      UF_long* jc = (UF_long*) cData->p;

      // iterate over columns
      for( int col = 0; col < n; col++ )
      {
         // iterate over nonzero rows
         for( int k = jc[col]; k < jc[col+1]; k++ )
         {
            int row = ir[k];

            (*this)( row, col ) = pr[k];
         }
      }

      return *this;
   }

   template <>
   const SparseMatrix<Complex>& SparseMatrix<Complex> :: operator=( cholmod_sparse* B )
   {
      assert( B );
      assert( B->xtype == CHOLMOD_COMPLEX );

      if( cData )
      {
         cholmod_l_free_sparse( &cData, context );
      }
      cData = B;

      m = cData->nrow;
      n = cData->ncol;
      resize( m, n );

      double* pr = (double*) cData->x;
      UF_long* ir = (UF_long*) cData->i;
      UF_long* jc = (UF_long*) cData->p;

      // iterate over columns
      for( int col = 0; col < n; col++ )
      {
         // iterate over nonzero rows
         for( int k = jc[col]; k < jc[col+1]; k++ )
         {
            int row = ir[k];

            (*this)( row, col ) = Complex( pr[k*2+0], pr[k*2+1] );
         }
      }

      return *this;
   }

   template <>
   cholmod_sparse* SparseMatrix<Quaternion> :: to_cholmod( void )
   {
      SparseMatrix<Real> A( m*4, n*4 );

      for( const_iterator e  = begin();
                          e != end();
                          e ++ )
      {
         int i = e->first.second;
         int j = e->first.first;
         const Quaternion& q( e->second );

         A(i*4+0,j*4+0) =  q[0]; A(i*4+0,j*4+1) = -q[1]; A(i*4+0,j*4+2) = -q[2]; A(i*4+0,j*4+3) = -q[3];
         A(i*4+1,j*4+0) =  q[1]; A(i*4+1,j*4+1) =  q[0]; A(i*4+1,j*4+2) = -q[3]; A(i*4+1,j*4+3) =  q[2];
         A(i*4+2,j*4+0) =  q[2]; A(i*4+2,j*4+1) =  q[3]; A(i*4+2,j*4+2) =  q[0]; A(i*4+2,j*4+3) = -q[1];
         A(i*4+3,j*4+0) =  q[3]; A(i*4+3,j*4+1) = -q[2]; A(i*4+3,j*4+2) =  q[1]; A(i*4+3,j*4+3) =  q[0];
      }

      if( cData != NULL )
      {
         cholmod_l_free_sparse( &cData, context );
      }
      cData = cholmod_l_copy_sparse( A.to_cholmod(), context );
      return cData;
   }

   template <>
   void SparseMatrix<Real> :: allocateSparse( void )
   {
      int nzmax = data.size();
      int sorted = true;
      int packed = true;
      int stype = 0;
      cData = cholmod_l_allocate_sparse( m, n, nzmax, sorted, packed, stype, CHOLMOD_REAL, context );
   }

   template <>
   void SparseMatrix<Complex> :: allocateSparse( void )
   {
      int nzmax = data.size();
      int sorted = true;
      int packed = true;
      int stype = 0;
      cData = cholmod_l_allocate_sparse( m, n, nzmax, sorted, packed, stype, CHOLMOD_COMPLEX, context );
   }

   template <>
   void SparseMatrix<Quaternion> :: allocateSparse( void )
   {
      int nzmax = data.size();
      int sorted = true;
      int packed = true;
      int stype = 0;
      cData = cholmod_l_allocate_sparse( m*4, n*4, nzmax, sorted, packed, stype, CHOLMOD_REAL, context );
   }

   template <>
   void SparseMatrix<Real> :: setEntry( const_iterator e, int i, double* pr )
   {
      pr[i] = e->second;
   }

   template <>
   void SparseMatrix<Complex> :: setEntry( const_iterator e, int i, double* pr )
   {
      pr[i*2+0] = e->second.re;
      pr[i*2+1] = e->second.im;
   }

   template <>
   void solve( SparseMatrix<Real>& A,
                DenseMatrix<Real>& x,
                DenseMatrix<Real>& b )
   // solves the sparse linear system Ax = b using sparse QR factorization
   {
      int t0 = clock();
      x = SuiteSparseQR<double>( A.to_cholmod(), b.to_cholmod(), context );
      int t1 = clock();

      cout << "[qr] time: " << seconds( t0, t1 ) << "s" << "\n";
      cout << "[qr] max residual: " << residual( A, x, b ) << "\n";
      cout << "[qr] size: " << A.nRows() << " x " << A.nColumns() << "\n";
      cout << "[qr] rank: " << (*context).SPQR_istat[4] << "\n";
   }

   template <>
   void solve( SparseMatrix<Complex>& A,
                DenseMatrix<Complex>& x,
                DenseMatrix<Complex>& b )
   // solves the sparse linear system Ax = b using sparse QR factorization
   {
      int t0 = clock();
      x = SuiteSparseQR< complex<double> >( A.to_cholmod(), b.to_cholmod(), context );
      int t1 = clock();

      cout << "[qr] time: " << seconds( t0, t1 ) << "s" << "\n";
      cout << "[qr] max residual: " << residual( A, x, b ) << "\n";
      cout << "[qr] size: " << A.nRows() << " x " << A.nColumns() << " (complex)" << "\n";
      cout << "[qr] rank: " << (*context).SPQR_istat[4] << "\n";
   }

   template <>
   void solve( SparseMatrix<Quaternion>& A,
                DenseMatrix<Quaternion>& x,
                DenseMatrix<Quaternion>& b )
   // solves the sparse linear system Ax = b using sparse QR factorization
   {
      int t0 = clock();
      x = SuiteSparseQR<double>( A.to_cholmod(), b.to_cholmod(), context );
      int t1 = clock();

      cout << "[qr] time: " << seconds( t0, t1 ) << "s" << "\n";
      cout << "[qr] max residual: " << residual( A, x, b ) << "\n";
      cout << "[qr] size: " << A.nRows() << " x " << A.nColumns() << " (quaternion)" << "\n";
      cout << "[qr] rank: " << (*context).SPQR_istat[4]/4 << "\n";
   }

   template <>
   void solveSymmetric( SparseMatrix<Complex>& A,
                        DenseMatrix<Complex>& x,
                        DenseMatrix<Complex>& b )
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

      umfpack_zl_symbolic( n, n, Ap, Ai, Ax, NULL, &Symbolic, NULL, NULL );
      umfpack_zl_numeric( Ap, Ai, Ax, NULL, Symbolic, &Numeric, NULL, NULL );
      umfpack_zl_solve( UMFPACK_A, Ap, Ai, Ax, NULL, (double*) &x(0), NULL, (double*) &b(0), NULL, Numeric, NULL, NULL );
      umfpack_zl_free_symbolic( &Symbolic );
      umfpack_zl_free_numeric( &Numeric );

      int t1 = clock();
      cout << "[lu] time: " << seconds( t0, t1 ) << "s" << "\n";
      cout << "[lu] max residual: " << residual( A, x, b ) << "\n";
   }
}
