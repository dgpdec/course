// -----------------------------------------------------------------------------
// libDDG -- HalfEdge.h
// -----------------------------------------------------------------------------
//
// HalfEdge is used to define mesh connectivity.  (See the documentation for a
// more in-depth discussion of the halfedge data structure.)
//

#ifndef DDG_HALFEDGE_H
#define DDG_HALFEDGE_H

#include "Vector.h"
#include "Types.h"

namespace DDG
{
   class HalfEdge
   {
   public:
      HalfEdgeIter next;
      // points to the next halfedge around the current face
      
      HalfEdgeIter flip;
      // points to the other halfedge associated with this edge
      
      VertexIter vertex;
      // points to the vertex at the "tail" of this halfedge
      
      EdgeIter edge;
      // points to the edge associated with this halfedge
      
      FaceIter face;
      // points to the face containing this halfedge
      
      bool onBoundary;
      // true if this halfedge is contained in a boundary
      // loop; false otherwise
      
      Vector texcoord;
      // texture coordinates associated with the triangle corner at the
      // "tail" of this halfedge
      
      double cotan( void ) const;
      // returns the cotangent of the angle opposing this edge
      
      Vector rotatedEdge( void ) const;
      // returns oriented edge vector rotated by PI/2 around face normal
      // if onBoundary, then return nil      
   };
}

#endif

