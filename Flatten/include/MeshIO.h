// -----------------------------------------------------------------------------
// libDDG -- MeshIO.h
// -----------------------------------------------------------------------------
//
// MeshIO handles input/output operations for Mesh objects.  Currently the only
// supported mesh format is Wavefront OBJ -- for a format specification see
//
//   http://en.wikipedia.org/wiki/Wavefront_.obj_file
//
// Note that vertex normals and material properties are currently ignored.
//

#ifndef DDG_MESHIO_H
#define DDG_MESHIO_H

#include <iosfwd>
#include <string>
#include <sstream>
#include <vector>

namespace DDG
{
   class Mesh;
   class Index;
   class MeshData;

   class MeshIO
   {
      public:
         static int read( std::istream& in, Mesh& mesh );
         // reads a mesh from a valid, open input stream in

         static void write( std::ostream& out, const Mesh& mesh );
         // writes a mesh to a valid, open output stream out

      protected:
         static  int readMeshData( std::istream& in, MeshData& data );
         static void readPosition( std::stringstream& ss, MeshData& data );
         static void readTexCoord( std::stringstream& ss, MeshData& data );
         static void readNormal  ( std::stringstream& ss, MeshData& data );
         static void readFace    ( std::stringstream& ss, MeshData& data );
         static Index parseFaceIndex( const std::string& token );
         static void preallocateMeshElements( const MeshData& data, Mesh& mesh );
         static  int buildMesh( const MeshData& data, Mesh& mesh );
         static void checkIsolatedVertices( const Mesh& Mesh );
         static void checkNonManifoldVertices( const Mesh& Mesh );
   };
}

#endif

