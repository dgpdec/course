/*
 * Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow
 * Keenan Crane, Clarisse Weischedel, Max Wardetzky
 * To appear at ACM Transactions on Graphics
 *
 * TODO: pre-factorize matrices
 */

#ifndef DDG_APPLICATION_H
#define DDG_APPLICATION_H

#include "Mesh.h"
#include "Real.h"
#include "Utility.h"
#include "DenseMatrix.h"
#include "SparseMatrix.h"
#include "DiscreteExteriorCalculus.h"

namespace DDG
{
   class Application
   {
   public:
      double run(double dt, Mesh& mesh)
      {
         // initial condiiton
         DenseMatrix<Real> u0;
         int nb = builImpulseSignal(mesh, u0);
         if( nb == 0 ) return 1.0;
         
         // DEC
         SparseMatrix<Real> star0;
         HodgeStar0Form<Real>::build( mesh, star0 );
         
         SparseMatrix<Real> star1;
         HodgeStar1Form<Real>::build( mesh, star1 );
         
         SparseMatrix<Real> d0;
         ExteriorDerivative0Form<Real>::build( mesh, d0 );
         
         // zero Neumann boundary condition
         SparseMatrix<Real> L = d0.transpose() * star1 * d0;
         
         // make L positive-definite
         L += Real(1.0e-8)*star0;
         
         // heat flow for short interval
         dt *= sqr(mesh.meanEdgeLength());
         SparseMatrix<Real> A = star0 + Real(dt) * L;

         DenseMatrix<Real> u;
         solvePositiveDefinite(A, u, u0);

         // extract geodesic
         computeVectorField(u, mesh);
         
         DenseMatrix<Real> div;
         computeDivergence(mesh, div);

         DenseMatrix<Real> phi;
         solvePositiveDefinite(L, phi, div);

         setMinToZero(phi);
         assignDistance(phi, mesh);         
         return phi.norm();
      }
      
   protected:
      int builImpulseSignal(const Mesh& mesh, DenseMatrix<Real>& x) const
      {
         int nb = 0;
         x = DenseMatrix<Real>(mesh.vertices.size());
         for( VertexCIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v ++ )
         {
            x(v->index) = 0.0;
            if( v->tag )
            {
               x(v->index) = 1.0;
               nb++;
            }
         }
         return nb;
      }
      
      void computeVectorField(const DenseMatrix<Real>& u, Mesh& mesh)
      {
         for( FaceIter f = mesh.faces.begin();
             f != mesh.faces.end();
             f++ )
         {
            if( f->isBoundary() ) continue;
            
            HalfEdgeIter hij = f->he;
            HalfEdgeIter hjk = hij->next;
            HalfEdgeIter hki = hjk->next;

            VertexIter vi = hij->vertex;
            VertexIter vj = hjk->vertex;
            VertexIter vk = hki->vertex;

            double ui = u(vi->index);
            double uj = u(vj->index);
            double uk = u(vk->index);

            Vector eij90 = hij->rotatedEdge();
            Vector ejk90 = hjk->rotatedEdge();
            Vector eki90 = hki->rotatedEdge();

            Vector X = 0.5 * ( ui*ejk90 + uj*eki90 + uk*eij90 ) / f->area();
            f->vector = - X.unit();
         }
      }
      
      void computeDivergence(const Mesh& mesh, DenseMatrix<Real>& div) const
      {
         div = DenseMatrix<Real>(mesh.vertices.size());
         for( VertexCIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v ++)
         {
            double sum = 0.0;
            HalfEdgeIter he = v->he;
            do
            {
               if( not he->onBoundary )
               {
                  Vector n = he->next->rotatedEdge();
                  Vector v = he->face->vector;
                  sum += dot( n, v );
               }
               he = he->flip->next;
            }
            while( he != v->he );
            div(v->index) = sum;
         }
      }
      
      void assignDistance(const DenseMatrix<Real>& phi, Mesh& mesh)
      {
         for( VertexIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v ++)
         {
            v->distance = phi(v->index);
         }
      }
      
      void setMinToZero(DenseMatrix<Real>& phi) const
      {
         double minValue = 1.0e100;
         for( int i = 0; i < phi.nRows(); ++i )
            for( int j = 0; j < phi.nColumns(); ++j )
               minValue = std::min( minValue, (double) phi(i,j) );

         for( int i = 0; i < phi.nRows(); ++i )
            for( int j = 0; j < phi.nColumns(); ++j )
               phi(i,j) -= minValue;
      }
   };
}
#endif
