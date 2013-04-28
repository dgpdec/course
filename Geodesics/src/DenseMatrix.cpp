#include "DenseMatrix.h"

namespace DDG
{
   template <>
   cholmod_dense* DenseMatrix<Real> :: to_cholmod( void )
   // returns pointer to underlying cholmod_dense data structure
   {
      if( cData )
      {
         cholmod_l_free_dense( &cData, context );
         cData = NULL;
      }

      int d = m; // leading dimension
      cData = cholmod_l_allocate_dense( m, n, d, CHOLMOD_REAL, context );
      double* x = (double*) cData->x;

      for( int i = 0; i < m*n; i++ )
      {
         x[i] = data[i];
      }

      return cData;
   }

   template <>
   cholmod_dense* DenseMatrix<Complex> :: to_cholmod( void )
   // returns pointer to underlying cholmod_dense data structure
   {
      if( cData )
      {
         cholmod_l_free_dense( &cData, context );
         cData = NULL;
      }

      int d = m; // leading dimension
      cData = cholmod_l_allocate_dense( m, n, d, CHOLMOD_COMPLEX, context );
      double* x = (double*) cData->x;

      for( int i = 0; i < m*n; i++ )
      {
         x[i*2+0] = data[i].re;
         x[i*2+1] = data[i].im;
      }

      return cData;
   }

   template <>
   cholmod_dense* DenseMatrix<Quaternion> :: to_cholmod( void )
   // returns pointer to underlying cholmod_dense data structure
   {
      assert( nColumns() == 1 );

      if( cData )
      {
         cholmod_l_free_dense( &cData, context );
         cData = NULL;
      }

      cData = cholmod_l_allocate_dense( m*4, 1, m*4, CHOLMOD_REAL, context );
      double* x = (double*) cData->x;

      for( int i = 0; i < m*n; i++ )
      {
         for( int k = 0; k < 4; k++ )
         {
            x[i*4+k] = data[i][k];
         }
      }

      return cData;
   }

   template <>
   const DenseMatrix<Real>& DenseMatrix<Real> :: operator=( cholmod_dense* B )
   // copies a cholmod_dense* into a DenseMatrix;
   // takes responsibility for deallocating B
   {
      assert( B );
      assert( B->xtype == CHOLMOD_REAL );

      if( cData )
      {
         cholmod_l_free_dense( &cData, context );
      }
      cData = B;
      
      m = cData->nrow;
      n = cData->ncol;
      data.resize( m*n );

      double* x = (double*) cData->x;
      for( int i = 0; i < m*n; i++ )
      {
         data[i] = x[i];
      }

      return *this;
   }

   template <>
   const DenseMatrix<Complex>& DenseMatrix<Complex> :: operator=( cholmod_dense* B )
   // copies a cholmod_dense* into a DenseMatrix;
   // takes responsibility for deallocating B
   {
      assert( B );
      assert( B->xtype == CHOLMOD_COMPLEX );

      if( cData )
      {
         cholmod_l_free_dense( &cData, context );
      }
      cData = B;

      m = cData->nrow;
      n = cData->ncol;
      data.resize( m*n );

      double* x = (double*) cData->x;
      for( int i = 0; i < m*n; i++ )
      {
         data[i] = Complex( x[i*2+0],
                            x[i*2+1] );
      }

      return *this;
   }

   template <>
   const DenseMatrix<Quaternion>& DenseMatrix<Quaternion> :: operator=( cholmod_dense* B )
   // copies a cholmod_dense* into a DenseMatrix;
   // takes responsibility for deallocating B
   {
      assert( B );
      assert( B->xtype == CHOLMOD_REAL );
      assert( B->ncol == 1 );
      assert( B->nrow%4 == 0 );

      if( cData )
      {
         cholmod_l_free_dense( &cData, context );
      }
      cData = B;

      m = cData->nrow/4;
      n = 1;
      data.resize( m*n );

      double* x = (double*) cData->x;
      for( int i = 0; i < m; i++ )
      {
         data[i] = Quaternion( x[i*4+0],
                               x[i*4+1],
                               x[i*4+2],
                               x[i*4+3] );
      }

      return *this;
   }

   template <>
   std::ostream& operator<< (std::ostream& os, const DenseMatrix<Real>& o)
   {
      const int p = 3;
      os.precision( p );
      os << scientific;

      for( int i = 0; i < o.nRows(); i++ )
      {
         os << "[ ";
         for( int j = 0; j < o.nColumns(); j++ )
         {
            double x = o(i,j);

            if( x == 0. )
            {
               os << " 0";
               for( int k = 0; k < p+6; k++ )
               {
                  os << " ";
               }
            }
            else if( x > 0. )
            {
               os << " " << x << " ";
            }
            else
            {
               os << x << " ";
            }
         }
         os << "]" << endl;
      }
   
      return os;
   }

   template <>
   std::ostream& operator<< (std::ostream& os, const DenseMatrix<Complex>& o)
   {
      const int p = 2;
      os.precision( p );
      os << scientific;

      for( int i = 0; i < o.nRows(); i++ )
      {
         os << "[ ";
         for( int j = 0; j < o.nColumns(); j++ )
         {
            Complex z = o(i,j);

            if( z.re == 0. )
            {
               os << " 0";
               for( int k = 0; k < p+5; k++ )
               {
                  os << " ";
               }
            }
            else if( z.re > 0. )
            {
               os << " " << z.re;
            }
            else
            {
               os << z.re;
            }

            if( z.im == 0 )
            {
               os << " ";
            }
            else if( z.im >= 0 )
            {
               os << "+";
            }
            else
            {
               os << "-";
            }

            if( z.im == 0. )
            {
               for( int k = 0; k < p+8; k++ )
               {
                  os << " ";
               }
            }
            else
            {
               os << abs( z.im ) << "i ";
            }
         }
         os << " ]" << endl;
      }
   
      return os;
   }

   template <>
   std::ostream& operator<< (std::ostream& os, const DenseMatrix<Quaternion>& o)
   {
      const int p = 2;
      os.precision( p );
      os << scientific;

      for( int i = 0; i < o.nRows(); i++ )
      {
         os << "[";
         for( int j = 0; j < o.nColumns(); j++ )
         {
            Quaternion q = o(i,j);

            os << " " << q;
         }
         os << " ]" << endl;
      }
   
      return os;
   }

   template <>
   void DenseMatrix<Real> :: randomize( void )
   // replaces entries with uniformly distributed real random numbers in the interval [-1,1]
   {
      for( int i = 0; i < m*n; i++ )
      {
         data[i] = 2.*unitRand() - 1.;
      }
   }

   template <>
   void DenseMatrix<Complex> :: randomize( void )
   // replaces entries with uniformly distributed real random numbers in the interval [-1,1]
   {
      for( int i = 0; i < m*n; i++ )
      {
         data[i].re = 2.*unitRand() - 1.;
         data[i].im = 2.*unitRand() - 1.;
      }
   }

   template <>
   void DenseMatrix<Quaternion> :: randomize( void )
   // replaces entries with uniformly distributed real random numbers in the interval [-1,1]
   {
      for( int i = 0; i < m*n; i++ )
      {
         for( int k = 0; k < 4; k++ )
         {
            data[i][k] = 2.*unitRand() - 1.;
         }
      }
   }
}

