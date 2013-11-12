/*
 * Spectral Conformal Parameterization
 * Patrick Mullen, Yiying Tong, Pierre Alliez, Mathieu Desbrun.
 * Symposium of Geometry Processing, 2008.
 */

#ifndef DDG_APPLICATION_H
#define DDG_APPLICATION_H

#include "Mesh.h"
#include "Complex.h"
#include "DenseMatrix.h"
#include "SparseMatrix.h"
#include "DiscreteExteriorCalculus.h"

namespace DDG
{
   class Application
   {
   public:
      void run(Mesh& mesh)
      {
         if (mesh.boundaries.empty())
         {
            std::cout << "Mesh has no boundary" << std::endl;
            return;
         }
         double initial_area = mesh.area();

         // Energy matrix
         SparseMatrix<Complex> Lc;
         buildEnergy(mesh, Lc);
         
         // make Lc positive-definite
         SparseMatrix<Complex> star0;
         HodgeStar0Form<Complex>::build( mesh, star0 );
         Lc += Complex(1.0e-8)*star0;
         
         // compute parameterization
         DenseMatrix<Complex> x(Lc.nRows());
         x.randomize();
         smallestEigPositiveDefinite(Lc, star0, x);
         assignSolution(x, mesh);
         
         // rescale mesh
         double scale = std::sqrt( initial_area / mesh.area() );
         normalizeMesh(scale, mesh);
      }
      
   protected:
      void buildEnergy(const Mesh& mesh, SparseMatrix<Complex>& A) const
      {
         // Laplacian
         SparseMatrix<Complex> d0, star1;
         HodgeStar1Form<Complex>::build( mesh, star1 );
         ExteriorDerivative0Form<Complex>::build( mesh, d0 );
         A = d0.transpose() * star1 * d0;
         
         // Area
         for( FaceCIter f = mesh.boundaries.begin();
             f != mesh.boundaries.end();
             f ++ )
         {
            HalfEdgeIter he = f->he;
            do
            {
               int i = he->flip->vertex->index;
               int j = he->vertex->index;

               A(i,j) += -0.5*DDGConstants::ii;
               A(j,i) +=  0.5*DDGConstants::ii;
               
               he = he->next;
            }
            while( he != f->he );
         }
      }
      
      void assignSolution(const DenseMatrix<Complex>& x, Mesh& mesh)
      {
         for( VertexIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v ++ )
         {
            int i = v->index;
            const Complex& z = x(i,0);
            v->texture.x = z.re;
            v->texture.y = z.im;
            v->texture.z = 0.0;
         }
      }
      
      void normalizeMesh(const double scale, Mesh& mesh)
      {
         Vector avg;
         for( VertexIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v ++ )
         {
            avg += v->texture;
         }
         avg /= mesh.vertices.size();
         
         for( VertexIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v ++ )
         {
            v->texture = scale*(v->texture - avg);
         }
      }
   };
}

#endif
