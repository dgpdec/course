/*
 * A Simple Geometric Model for Elastic Deformations
 * Isaac Chao, Ulrich Pinkall, Patrick Sanan, Peter Schröder
 * ACM Transactions on Graphics, 29(4), 2010, 38:1-38:6.
 *
 * TODO: implemented just a gradient descent; use full Newton's method
 *
 */

#ifndef DDG_APPLICATION_H
#define DDG_APPLICATION_H

#include "Mesh.h"
#include "Complex.h"
#include "DenseMatrix.h"
#include "SparseMatrix.h"
#include "DiscreteExteriorCalculus.h"
#include "PolarDecomposition2x2.h"

namespace DDG
{
   class Application
   {
   public:
      void interpolate(const double t,
                       const Mesh& source,
                       const Mesh& target,
                       Mesh& mesh,
                       int max_iters = 20,
                       double tolerance = 1.0e-8)
      {
         SparseMatrix<Complex> src_L;
         computeLaplacian(source, src_L);
         
         SparseMatrix<Complex> tgt_L;
         computeLaplacian(target, tgt_L);
         
         SparseMatrix<Complex> L  = Complex(1.-t)*src_L   + Complex(t)*tgt_L;
         SparseFactor<Complex> LL;
         LL.build( L );

         for( int iter = 0; iter < max_iters; iter++ )
         {
            // extract rotation from deformation gradient
            DenseMatrix<Complex> src_angle;
            computeRotation(source, mesh, src_angle);
            
            DenseMatrix<Complex> tgt_angle;
            computeRotation(target, mesh, tgt_angle);
            
            // optimize vertices' position
            DenseMatrix<Complex> src_rhs;
            computeDivergence(source, src_angle, src_rhs);
            
            DenseMatrix<Complex> tgt_rhs;
            computeDivergence(target, tgt_angle, tgt_rhs);
            
            DenseMatrix<Complex> rhs = Complex(1.-t)*src_rhs + Complex(t)*tgt_rhs;
            
            DenseMatrix<Complex> x;
            get2DPositions(mesh, x);
                        
            double res = residual(L, x, rhs);
            std::cout << "Iter" << iter << ": res = " << res << std::endl;
            if (res < tolerance) break;
            
            backsolvePositiveDefinite(LL, x, rhs);
            x.removeMean();
            assign2DPositions(x, mesh);
         }
      }
      
      void init(const double step,
                const Mesh& source,
                const Mesh& target,
                Mesh& mesh)
      {
         VertexCIter sv = source.vertices.begin();
         VertexCIter tv = target.vertices.begin();
         VertexIter  v  = mesh.vertices.begin();
         for( ; v != mesh.vertices.end(); v++, tv++, sv++)
         {
            v->position = (1.-step)*sv->position + step*tv->position;
         }
      }
      
   protected:
      void computeRotation(const Mesh& meshA,
                           const Mesh& meshB,
                           DenseMatrix<Complex>& angle) const
      {
         angle = DenseMatrix<Complex>(meshA.faces.size(),1);
         FaceCIter fA = meshA.faces.begin();
         FaceCIter fB = meshB.faces.begin();
         for( ; fA != meshA.faces.end(); fA++, fB++)
         {
            // edge vectors from A
            VertexIter v0 = fA->he->vertex;
            VertexIter v1 = fA->he->next->vertex;
            VertexIter v2 = fA->he->next->next->vertex;
            
            Vector e01 = v1->position - v0->position;
            Vector e12 = v2->position - v1->position;

            DenseMatrix<Real> A0(2,2);
            A0(0,0) = e01.x; A0(0,1) = e12.x;
            A0(1,0) = e01.y; A0(1,1) = e12.y;
            
            // edge vectors from B
            v0 = fB->he->vertex;
            v1 = fB->he->next->vertex;
            v2 = fB->he->next->next->vertex;
            
            e01 = v1->position - v0->position;
            e12 = v2->position - v1->position;
            
            DenseMatrix<Real> A1(2,2);
            A1(0,0) = e01.x; A1(0,1) = e12.x;
            A1(1,0) = e01.y; A1(1,1) = e12.y;
            
            // ortho(A1*inv(A0))
            DenseMatrix<Real> M = A1 * PolarDecomposition2x2::invert(A0);
            Complex z = PolarDecomposition2x2::extractOrthogonalPart(M);
            angle(fA->index,0) = z;
         }
      }
      
      void computeLaplacian(const Mesh& mesh,
                            SparseMatrix<Complex>& L) const
      {
         SparseMatrix<Complex> star0;
         SparseMatrix<Complex> star1;
         HodgeStar0Form<Complex>::build( mesh, star0 );
         HodgeStar1Form<Complex>::build( mesh, star1 );
                  
         SparseMatrix<Complex> d0;
         ExteriorDerivative0Form<Complex>::build( mesh, d0 );
         
         L = d0.transpose() * star1 * d0;
         L += Complex(1e-8)*star0;
      }
      
      void computeDivergence(const Mesh& mesh,
                             const DenseMatrix<Complex>& angle,
                             DenseMatrix<Complex>& rhs) const
      {
         rhs = DenseMatrix<Complex>(mesh.vertices.size(),1);
         for( VertexCIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v ++ )
         {
            Complex sum;
            Vector pi = v->position;
            HalfEdgeIter he = v->he;
            do
            {
               if( not he->onBoundary )
               {
                  Vector area_grad = 0.5*he->next->rotatedEdge();
                  Complex sum_ijk( area_grad.x, area_grad.y );
                  Complex Rijk = angle(he->face->index, 0);
                  sum += Rijk * sum_ijk;
               }
               he = he->flip->next;
            }
            while( he != v->he );
            rhs(v->index,0) = sum;
         }
      }
      
      void assign2DPositions(const DenseMatrix<Complex>& x, Mesh& mesh)
      {
         for( VertexIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v ++ )
         {
            Complex xi = x(v->index,0);
            v->position = Vector(xi.re, xi.im, 0.);
         }
      }
      
      void get2DPositions(const Mesh& mesh, DenseMatrix<Complex>& x) const
      {
         x = DenseMatrix<Complex>(mesh.vertices.size(),1);
         for( VertexCIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v ++ )
         {
            Complex z(v->position.x, v->position.y);
            x(v->index,0) = z;
         }
      }
   };
}
#endif
