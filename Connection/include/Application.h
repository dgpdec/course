/*
 * Trivial Connections on Discrete Surfaces
 * Keenan Crane, Mathieu Desbun and Peter Schroeder
 * SGP 2010 / Computer Graphics Forum
 *
 * A Simplified Algorithm for Simply-Connected Surfaces 
 * Fernando de Goes and Keenan Crane
 *
 * TODO: add soft and hard directional constraints
 */

#ifndef DDG_APPLICATION_H
#define DDG_APPLICATION_H

#include "Mesh.h"
#include "Real.h"
#include "Quaternion.h"
#include "DenseMatrix.h"
#include "SparseMatrix.h"
#include "DiscreteExteriorCalculus.h"
#include "Utility.h"

namespace DDG
{
   class Application
   {
   public:
      bool solveForConneciton(Mesh& mesh)
      {
         bool ok = checkGaussBonnet(mesh);
         if( not ok )
         {
            std::cout << "Gauss-Bonnet thm does not hold" << std::endl;
            return false;
         }
         
         int t0 = clock();
         solveForTrivialHolonomy(mesh);
         int t1 = clock();
         cout << "[trivial] time: " << seconds( t0, t1 ) << "s" << "\n";

         t0 = clock();
         solveForNonTrivialHolonomy(mesh);
         t1 = clock();
         cout << "[nontrivial] time: " << seconds( t0, t1 ) << "s" << "\n";
         
         return true;
      }
      
   protected:
      bool checkGaussBonnet(const Mesh& mesh) const
      {
         // vertex singularity
         int k = 0;
         for(VertexCIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v++)
         {
            k += v->singularity;
         }
         
         // generator singularity
         // TODO: include singularity for all generators
         k += mesh.firstGeneratorIndex;
         
         return ( mesh.getEulerCharacteristicNumber() == k );
      }
      
      void solveForTrivialHolonomy(Mesh& mesh)
      {
         // Neumann boundary condition => prescribing geodesic curvature
         // For now, keeping original geodesic curvature
         DenseMatrix<Real> b( mesh.vertices.size() );
         for(VertexIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v++)
         {
            double value = 0.0;
            if( not v->onBoundary() )
            {
               value -= ( 2. * M_PI - v->theta() );
               value += 2. * M_PI * v->singularity;
            }
            b( v->index ) = value;
         }
         
         DenseMatrix<Real> u( mesh.vertices.size() );
         if( b.norm() > 1.0e-8 ) backsolvePositiveDefinite( mesh.L, u, b );

         for(VertexIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v++)
         {
            v->potential = u( v->index );
         }
      }

      void solveForNonTrivialHolonomy(Mesh& mesh)
      {
         unsigned nb = mesh.numberHarmonicBases();
         if( nb == 0 ) return;
         mesh.harmonicCoefs = std::vector<double>(nb, 0.0);

         DenseMatrix<Real>  b(nb);
         SparseMatrix<Real> H(nb,nb);
         
         int row = 0;
         bool skipBoundaryLoop = true;
         for(unsigned i = 0; i < mesh.generators.size(); ++i)
         {
            const Mesh::Generator& cycle = mesh.generators[i];
            if( skipBoundaryLoop and mesh.isBoundaryGenerator(cycle) )
            {
               skipBoundaryLoop = false;
               continue;
            }
            
            for(unsigned j = 0; j < cycle.size(); ++j)
            {
               HalfEdgeIter he = cycle[j];
               for(unsigned col = 0; col < nb; ++col)
               {
                  H(row,col) += he->harmonicBases[col];
               }
            }

            double value = - mesh.generatorHolonomy( cycle );
            if( row == 0 )
            {
               value += 2.0 * M_PI * mesh.firstGeneratorIndex ;
            }
            b(row) = value;
            row++;
         }
         
         DenseMatrix<Real> x(nb);
         if( b.norm() > 1.0e-8 ) solve(H, x, b);

         for(unsigned i = 0; i < nb; ++i)
            mesh.harmonicCoefs[i] = x(i);
      }
   };
}

#endif
