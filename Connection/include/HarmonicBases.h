#ifndef DDG_HARMONIC_BASES_H
#define DDG_HARMONIC_BASES_H

#include "Mesh.h"
#include "Real.h"
#include "DenseMatrix.h"
#include "SparseMatrix.h"
#include "DiscreteExteriorCalculus.h"

namespace DDG
{
   class HarmonicBases
   {
   public:
      void compute(Mesh& mesh)
      {
         cleanHalfEdges( mesh );
         if( mesh.generators.empty() ) return;

         SparseMatrix<Real> star1, d0, div;
         HodgeStar1Form<Real>::build( mesh, star1 );
         ExteriorDerivative0Form<Real>::build( mesh, d0 );
         div = d0.transpose() * star1;
         
         bool skipBoundaryLoop = true;
         for(unsigned i = 0; i < mesh.generators.size(); ++i)
         {
            const Mesh::Generator& cycle = mesh.generators[i];
            if( skipBoundaryLoop and mesh.isBoundaryGenerator(cycle) )
            {
               skipBoundaryLoop = false;
               continue;
            }
            
            DenseMatrix<Real> w;
            buildClosedPrimalOneForm(mesh, cycle, w);
                        
            DenseMatrix<Real> u;
            DenseMatrix<Real> divw = div * w;
            backsolvePositiveDefinite( mesh.L, u, divw );
            
            DenseMatrix<Real> h = star1*( w - (d0*u) );
            storeHarmonicForm(h, mesh);
          }
      }
      
   protected:
      void buildClosedPrimalOneForm(const Mesh& mesh,
                                    const Mesh::Generator& cycle,
                                    DenseMatrix<Real>& oneform) const
      {
         oneform = DenseMatrix<Real>( mesh.edges.size() );
         for(unsigned i = 0; i < cycle.size(); ++i)
         {
            double value = 1.0;
            HalfEdgeIter he = cycle[i];
            if( he->edge->he != he ) value = -value;
            oneform( he->edge->index ) = value;
         }
      }
      
      void storeHarmonicForm(const DenseMatrix<Real>& oneform,
                             Mesh& mesh)
      {
         for(EdgeIter e = mesh.edges.begin();
             e != mesh.edges.end();
             e ++)
         {
            double value = oneform(e->index);
            e->he->harmonicBases.push_back( value );
            e->he->flip->harmonicBases.push_back( -value );
         }
      }
      
      void cleanHalfEdges(Mesh& mesh)
      {
         for(EdgeIter e = mesh.edges.begin();
             e != mesh.edges.end();
             e ++)
         {
            e->he->harmonicBases.clear();
            e->he->flip->harmonicBases.clear();
         }
      }
   };
}

#endif