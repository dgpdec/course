#include "HalfEdge.h"
#include "Mesh.h"

namespace DDG
{
   double HalfEdge :: cotan( void ) const
   {
      if( onBoundary ) return 0.0;

      Vector p0 = next->next->vertex->position;
      Vector p1 = vertex->position;
      Vector p2 = next->vertex->position;

      Vector u = p1-p0;
      Vector v = p2-p0;

      return dot( u, v ) / cross( u, v ).norm();
   }

   Vector HalfEdge :: rotatedEdge( void ) const
   {
      if( onBoundary ) return Vector();
      
      Vector n = face->normal();
      Vector p0 = vertex->position;
      Vector p1 = flip->vertex->position;
      return cross( n, p1-p0 );
   }

   double HalfEdge :: height( void ) const
   {
      if( onBoundary ) return 0.0;
      
      double cotk = cotan();
      double coti = next->cotan();
      double cotj = next->next->cotan();
      
      double wk = next->next->vertex->weight;
      double wi = vertex->weight;
      double wj = next->vertex->weight;
      
      Vector pi = vertex->position;
      Vector pj = next->vertex->position;
      double lij = (pj - pi).norm();
      
      return 0.5*(lij*cotk + (wi-wk)*cotj/lij + (wj-wk)*coti/lij);
   }
   
   double HalfEdge :: shift( void ) const
   {
      double wi = vertex->weight;
      double wj = flip->vertex->weight;
      
      Vector pi = vertex->position;
      Vector pj = flip->vertex->position;
      double lij = (pj - pi).norm();
      
      return 0.5*(lij + (wi-wj)/lij);
   }
}

