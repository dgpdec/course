// -----------------------------------------------------------------------------
// libDDG -- Vertex.h
// -----------------------------------------------------------------------------
//
// Vertex stores attributes associated with a mesh edge.  The iterator he
// points to its "outgoing" halfedge.  (See the documentation for a more
// in-depth discussion of the halfedge data structure.)
//

#ifndef DDG_VERTEX_H
#define DDG_VERTEX_H

#include "Vector.h"
#include "Types.h"

namespace DDG
{
   class Vertex
   {
   public:
      HalfEdgeIter he;
      // points to the "outgoing" halfedge
      
      Vector texture;
      // location of vertex in Euclidian 2-space

      Vector position;
      // location of vertex in Euclidean 3-space
      
      int index;
      // unique integer ID in the range 0, ..., nVertices-1
      
      bool tag;
      // true if vertex is selected by the user; false otherwise

      Vertex() : index(0), tag(false) { }
      
      double area( void ) const;
      // returns the barycentric area associated with this vertex
      
      Vector normal( void ) const;
      // returns the vertex normal
      
      bool isIsolated( void ) const;
      // returns true if the vertex is not contained in any face or edge; false otherwise
      
      int valence( void ) const;
      // returns the number of incident faces / edges
      
      void toggleTag();
      // toggle vertex tag 
   };
}

#endif

