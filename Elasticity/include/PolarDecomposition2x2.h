#ifndef POLAR_DECOMPOSITION_2x2_H
#define POLAR_DECOMPOSITION_2x2_H

#include "Complex.h"
#include "DenseMatrix.h"

namespace DDG
{
   class PolarDecomposition2x2
   {
   public:
      static Complex extractOrthogonalPart(DenseMatrix<Real> A)
      {
         double res = 1.0;
         while( res > 1.0e-6 )
         {
            DenseMatrix<Real> B = Real(0.5)*( A + invert(A.transpose()) );
            res = (A - B).norm( lTwo );
            A = B;
         }
         return Complex( A(0,0), A(1,0) ).unit();
      }
      
      static DenseMatrix<Real> invert(const DenseMatrix<Real>& A)
      {
         double detA = A(0,0)*A(1,1) - A(0,1)*A(1,0);
         DenseMatrix<Real> B(2,2);
         B(0,0) =  A(1,1)/detA; B(0,1) = -A(0,1)/detA;
         B(1,0) = -A(1,0)/detA; B(1,1) =  A(0,0)/detA;
         return B;
      }
   };
}

#endif
