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

namespace DDG
{
   class Application
   {
   public:
      void solveForConneciton(Mesh& mesh)
      {
         // For simplicity, tagged vertices have equal index
         setConstantIndex(mesh);
         
         solveForCurvature(mesh);
      }
      
   protected:
      void setConstantIndex(Mesh& mesh)
      {
         int nbSingularities = 0;
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
         if( std::abs(insideCurvature) > 1.0e-8 )
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
      
      void solveForCurvature(Mesh& mesh)
      {
         // RHS
         DenseMatrix<Real> b( mesh.vertices.size() );
         bool ok = buildAngleDefect( mesh, b );
         
         if( not ok )
         {
            std::cout << "Gauss-Bonnet thm does not hold" << std::endl;
            return;
         }
         
         if( b.norm() < 1.0e-8 )
         {
            std::cout << "no curvature change" << std::endl;
            return;
         }
         
         // Potential
         DenseMatrix<Real> u;
         backsolvePositiveDefinite( mesh.L, u, b );
         assignPotential( u, mesh );
      }
      
      bool buildAngleDefect(const Mesh& mesh, DenseMatrix<Real>& b) const
      {
         // Neumann boundary condition => prescibing geodesic curvature
         // For now, keeping original geodesic curvature
         double sum = 0.0;
         b = DenseMatrix<Real>( mesh.vertices.size() );
         for( VertexCIter v = mesh.vertices.begin();
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
   };
}

#endif
