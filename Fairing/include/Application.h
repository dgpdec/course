/*
 * Implicit Fairing of Arbitrary Meshes using Diffusion and Curvature Flow
 * Mathieu Desbrun, Mark Meyer, Peter Schröder, Alan H. Barr
 * ACM Siggraph '99 Proceedings
 */

#ifndef DDG_APPLICATION_H
#define DDG_APPLICATION_H

#include "Mesh.h"
#include "Real.h"
#include "DenseMatrix.h"
#include "SparseMatrix.h"
#include "DiscreteExteriorCalculus.h"

namespace DDG
{
   class Application
   {
   public:
      void run(const double step, Mesh& mesh)
      {
         SparseMatrix<Real> star0;
         HodgeStar0Form<Real>::build( mesh, star0 );

         SparseMatrix<Real> star1;
         HodgeStar1Form<Real>::build( mesh, star1 );
         
         SparseMatrix<Real> d0;
         ExteriorDerivative0Form<Real>::build( mesh, d0 );
         
         SparseMatrix<Real> L = d0.transpose() * star1 * d0;
         SparseMatrix<Real> A = star0 + Real(step) * L;
         
         DenseMatrix<Real> x;
         getPositions(mesh, x);
         DenseMatrix<Real> rhs = star0 * x;
         
         solvePositiveDefinite(A, x, rhs);
         setPositions(x, mesh);
      }
      
   protected:
      void getPositions(const Mesh& mesh, DenseMatrix<Real>& x) const
      {
         x = DenseMatrix<Real>( mesh.vertices.size(), 3 );
         for ( VertexCIter v = mesh.vertices.begin();
              v != mesh.vertices.end();
              v ++)
         {
            for( int i = 0; i < 3; ++i)
               x(v->index, i) = v->position[i];
         }
      }
      
      void setPositions(const DenseMatrix<Real>& x, Mesh& mesh)
      {
         for ( VertexIter v = mesh.vertices.begin();
              v != mesh.vertices.end();
              v ++)
         {
            v->position = Vector(x(v->index, 0),
                                 x(v->index, 1),
                                 x(v->index, 2));
         }
      }
   };
}

#endif
