/*
 * Trivial Connections on Discrete Surfaces
 * Keenan Crane, Mathieu Desbun and Peter Schroeder
 * SGP 2010 / Computer Graphics Forum
 *
 * A Simplified Algorithm for Simply-Connected Surfaces 
 * Fernando de Goes and Keenan Crane
 *
 * TODO: gui to edit singularity indices
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
         // For simplicity, tagged vertices have equal index
         setConstantIndex(mesh);
         
         int t0 = clock();
         bool ok = solveForTrivialHolonomy(mesh);
         int t1 = clock();
         cout << "[trivial] time: " << seconds( t0, t1 ) << "s" << "\n";

         if( not ok ) return false;
         
         t0 = clock();
         solveForNonTrivialHolonomy(mesh);
         t1 = clock();
         cout << "[nontrivial] time: " << seconds( t0, t1 ) << "s" << "\n";
         
         return true;
      }
      
   protected:
      void setConstantIndex(Mesh& mesh)
      {
         unsigned nbSingularities = 0;
         double insideCurvature = 0.0;
         for( VertexCIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v++)
         {
            if( not v->onBoundary() )
            {
               insideCurvature += ( 2.*M_PI - v->theta() );
               if( v->tag ) nbSingularities++;
            }
         }
         
         double index = 0.0;
         if( nbSingularities > 0 )
         {
            index = insideCurvature / ( 2.*M_PI*nbSingularities );
         }
         
         for( VertexIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v++)
         {
            double singularity = 0.0;
            if( v->tag ) singularity = index;
            v->singularity = singularity;
         }
      }
      
      bool solveForTrivialHolonomy(Mesh& mesh)
      {
         DenseMatrix<Real> b;
         bool ok = buildAngleDefect( mesh, b );
         
         if( not ok )
         {
            std::cout << "Gauss-Bonnet thm does not hold" << std::endl;
            return false;
         }
         
         if( b.norm() < 1.0e-8 )
         {
            std::cout << "no curvature change" << std::endl;
            return false;
         }
         
         DenseMatrix<Real> u;
         backsolvePositiveDefinite( mesh.L, u, b );
         assignPotential( u, mesh );
         return true;
      }
      
      bool buildAngleDefect(Mesh& mesh, DenseMatrix<Real>& b) const
      {
         // Neumann boundary condition => prescibing geodesic curvature
         // For now, keeping original geodesic curvature
         double sum = 0.0;
         b = DenseMatrix<Real>( mesh.vertices.size() );
         for( VertexIter v = mesh.vertices.begin();
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
            sum += value;
         }
         return ( std::abs(sum) < 1.0e-8 );
      }
      
      void assignPotential(const DenseMatrix<Real>& u, Mesh& mesh)
      {
         for( VertexIter v = mesh.vertices.begin();
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

            b(row) = - mesh.generatorHolonomy(cycle);
            row++;
         }
         
         DenseMatrix<Real> x;
         solve(H, x, b);

         mesh.harmonicCoefs = std::vector<double>( nb , 0.0 );
         for(unsigned i = 0; i < nb; ++i)
         {
            mesh.harmonicCoefs[i] = x(i);
         }
      }
   };
}

#endif
