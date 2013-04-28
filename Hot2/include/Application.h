/*
 * HOT: Hodge-Optimized Triangulations
 * Patrick Mullen, Pooran Memari, Fernando de Goes, Mathieu Desbrun.
 * Transactions on Graphics (SIGGRAPH) 2011.
 */

#ifndef DDG_APPLICATION_H
#define DDG_APPLICATION_H

#include "Mesh.h"
#include "DenseMatrix.h"
#include "SparseMatrix.h"
#include "DiscreteExteriorCalculus.h"

namespace DDG
{
   class Application
   {
   public:
      void optimizeWeights(Mesh& mesh)
      {
         SparseMatrix<Real> d0, star0, star1, Delta;
         HodgeStar1Form<Real>::build( mesh, star1 );
         HodgeStar0Form<Real>::build( mesh, star0 );
         ExteriorDerivative0Form<Real>::build( mesh, d0 );
         Delta = d0.transpose() * star1 * d0;
         Delta += Real(1e-8)*star0;

         DenseMatrix<Real> rhs;
         buildRhs(mesh, rhs);
         
         DenseMatrix<Real> x;
         solvePositiveDefinite(Delta, x, rhs);
         
         assignSolution(mesh, x);
      }
      
   protected:
      void buildRhs(const Mesh& mesh, DenseMatrix<Real>& rhs) const
      {
         rhs = DenseMatrix<Real>(mesh.vertices.size(),1);
         for( VertexCIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v ++)
         {
            double sum = 0.0;
            HalfEdgeIter he = v->he;
            do
            {
               if( !he->onBoundary )
               {
                  Vector n = he->next->rotatedEdge();
                  Vector c = he->face->circumcenter();
                  Vector b = he->face->barycenter();
                  sum += dot( n, c-b );
               }
               he = he->flip->next;
            }
            while( he != v->he );
            rhs(v->index,0) = sum;
         }
      }
      
      void assignSolution(Mesh& mesh, const DenseMatrix<Real>& x) const
      {
         for( VertexIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v ++)
         {
            v->weight = x(v->index,0);
         }
      }
   };
}

#endif
