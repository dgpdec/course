// -----------------------------------------------------------------------------
// libDDG -- Mesh.h
// -----------------------------------------------------------------------------
//
// Mesh represents a polygonal surface mesh using the halfedge data structure.
// It is essentially a large collection of disjoint vertices, edges, and faces
// that are ``glued together'' by halfedges which encode connectivity (see
// the documentation for an illustration).  By construction, the halfedge data
// structure cannot represent nonorientable surfaces or meshes with nonmanifold
// edges.
//
// Mesh elements are referenced using iterators -- common usage of these
// iterators is to either traverse an entire vector of mesh elements:
//
//    // visit all vertices
//    for( VertexIter i = vertices.begin(); i != vertices.end(); i++ )
//    {
//       //...
//    }
//
// or to perform a local traversal over the neighborhood of some mesh element:
//
//    // visit both halfedges of edge e
//    HalfEdgeIter he = e->he;
//    do
//    {
//       // ...
//
//       he = he->flip;
//    }
//    while( he != e->he );
//
// (See Types.h for an explicit definition of iterator types.)
//
// Meshes with boundary are handled by creating an additional face for each
// boundary loop (the method Face::isBoundary() determines whether a given
// face is a boundary loop).  Isolated vertices (i.e., vertiecs not contained
// in any edge or face) reference a dummy halfedge and can be checked via
// the method Vertex::isIsolated().
//

#ifndef DDG_MESH_H
#define DDG_MESH_H

#include <vector>
#include <string>

#include "HalfEdge.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
#include "SparseMatrix.h"

namespace DDG
{
   class Mesh
   {
   public:
      Mesh( void );
      // constructs an empty mesh
      
      Mesh( const Mesh& mesh );
      // constructs a copy of mesh
      
      const Mesh& operator=( const Mesh& mesh );
      // copies mesh
      
      int read( const std::string& filename );
      // reads a mesh from a Wavefront OBJ file; return value is nonzero
      // only if there was an error
      
      int write( const std::string& filename ) const;
      // writes a mesh to a Wavefront OBJ file; return value is nonzero
      // only if there was an error
      
      bool reload( void );
      // reloads a mesh from disk using the most recent input filename
      
      void normalize( void );
      // centers around the origin and rescales to have unit radius
      
      double area( void ) const;
      // returns total mesh area
      
      double meanEdgeLength( void  ) const;
      // returns mean edge lenght

      std::vector<HalfEdge> halfedges;
      std::vector<Vertex>   vertices;
      std::vector<Edge>     edges;
      std::vector<Face>     faces;
      std::vector<Face>     boundaries;
      // storage for mesh elements
      
   protected:
      std::string inputFilename;
      
      void indexElements( void );
      // assigns a unique, 0-based index to each mesh element
   };
}

#endif

