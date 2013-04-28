#include <vector>
#include <iostream>
#include <map>
#include <set>

#include "MeshIO.h"
#include "Mesh.h"

using namespace std;

namespace DDG
{
   class Index
   {
      public:
         Index( void )
         {}
   
         Index( int p, int t, int n )
         : position( p ), texcoord( t ), normal( n )
         {}
   
         bool operator<( const Index& i ) const
         {
            if( position < i.position ) return true;
            if( position > i.position ) return false;
            if( texcoord < i.texcoord ) return true;
            if( texcoord > i.texcoord ) return false;
            if(   normal < i.normal   ) return true;
            if(   normal > i.normal   ) return false;
            return false;
         }
   
         int position;
         int texcoord;
         int normal;
   };
   
   class MeshData
   {
      public:
         std::vector<Vector> positions;
         std::vector<Vector> texcoords;
         std::vector<Vector> normals;
         std::vector< std::vector< Index > > indices;
   };
   
   int MeshIO :: read( istream& in, Mesh& mesh )
   // reads a mesh from a valid, open input stream in
   {
      MeshData data;
   
      if( readMeshData( in, data ))
      {
         return 1;
      }

      if( buildMesh( data, mesh ))
      {
         return 1;
      }

      return 0;
   }
   
   void MeshIO :: write( ostream& out, const Mesh& mesh )
   // writes a mesh to a valid, open output stream out
   {
      int currentIndex = 1;
      map<VertexCIter,int> vertexIndex;
   
      for( VertexCIter v  = mesh.vertices.begin();
                       v != mesh.vertices.end();
                       v++ )
      {
         out << "v " << v->position.x << " "
                     << v->position.y << " "
                     << v->position.z << endl;
   
         vertexIndex[ v ] = currentIndex;
         currentIndex++;
      }

      for( FaceCIter f  = mesh.faces.begin();
                     f != mesh.faces.end();
                     f ++ )
      {
         HalfEdgeIter he = f->he;
   
         for( int j = 0; j < 3; j++ )
         {
            out << "vt " << he->texcoord.x << " " << he->texcoord.y << endl;
            he = he->next;
         }
      }
   
      for( size_t i = 0; i < mesh.faces.size(); i++ )
      {
         const Face& f( mesh.faces[i] );
         HalfEdgeIter he = f.he;
   
         out << "f ";
   
         int j = 0;
         do
         {
            out << vertexIndex[ he->vertex ] << "/" << 1+(i*3+j) << " ";
            he = he->next;
            j++;
         }
         while( he != f.he );
   
         out << endl;
      }
   }
   
   int MeshIO :: readMeshData( istream& in, MeshData& data )
   {
      string line;
   
      while( getline( in, line ))
      {
         stringstream ss( line );
         string token;
   
         ss >> token;
   
         if( token == "v"  ) { readPosition( ss, data ); continue; } // vertex
         if( token == "vt" ) { readTexCoord( ss, data ); continue; } // texture coordinate
         if( token == "vn" ) { readNormal  ( ss, data ); continue; } // vertex normal
         if( token == "f"  ) { readFace    ( ss, data ); continue; } // face
         if( token[0] == '#' ) continue; // comment
         if( token == "o" ) continue; // object name
         if( token == "g" ) continue; // group name
         if( token == "s" ) continue; // smoothing group
         if( token == "mtllib" ) continue; // material library
         if( token == "usemtl" ) continue; // material
         if( token == "" ) continue; // empty string

         cerr << "Error: does not appear to be a valid Wavefront OBJ file!" << endl;
         cerr << "(Offending line: " << line << ")" << endl;
         return 1;
      }

      return 0;
   }
   
   void MeshIO :: preallocateMeshElements( const MeshData& data, Mesh& mesh )
   {
      // count the number of edges
      set< pair<int,int> > edges;
      for( vector< vector< Index > >::const_iterator f  = data.indices.begin();
                                                     f != data.indices.end();
                                                     f ++ )
      {
         for( unsigned int I = 0; I < f->size(); I++ )
         {
            int J = (I+1) % f->size();
            int i = (*f)[I].position;
            int j = (*f)[J].position;
   
            if( i > j ) swap( i, j );
   
            edges.insert( pair<int,int>( i, j ));
         }
      }
   
      int nV = data.positions.size();
      int nE = edges.size();
      int nF = data.indices.size();
      int nHE = 2*nE;
      int chi = nV - nE + nF;
      int nB = max( 0, 2 - chi ); // (conservative approximation of number of boundary cycles)

      mesh.halfedges.clear();
      mesh.vertices.clear();
      mesh.edges.clear();
      mesh.faces.clear();
      mesh.boundaries.clear();
   
      mesh.halfedges.reserve( nHE );
      mesh.vertices.reserve( nV );
      mesh.edges.reserve( nE );
      mesh.faces.reserve( nF );
      mesh.boundaries.reserve( nB );
   }

   extern vector<HalfEdge> isolated; // all isolated vertices point to isolated.begin()
   
   int MeshIO :: buildMesh( const MeshData& data, Mesh& mesh )
   {
      map< pair< int, int >, int > edgeCount;
      map< pair< int, int >, HalfEdgeIter > existingHalfEdges;
      map< int, VertexIter > indexToVertex;
      map< HalfEdgeIter, bool > hasFlipEdge;
   
      preallocateMeshElements( data, mesh );
   
      // allocate a vertex for each position in the data and construct
      // a map from vertex indices to vertex pointers
      for( unsigned int i = 0; i < data.positions.size(); i++ )
      {
         VertexIter newVertex = mesh.vertices.insert( mesh.vertices.end(), Vertex() );
         newVertex->position = data.positions[ i ];
         newVertex->he = isolated.begin();
         indexToVertex[ i ] = newVertex;
      }
   
      // insert each face into the mesh
      int faceIndex = 0;
      bool degenerateFaces = false;
      for( vector< vector< Index > >::const_iterator f  = data.indices.begin();
                                                     f != data.indices.end();
                                                     f ++ )
      {
         int N = f->size();

         // print an error if the face is degenerate
         if( N < 3 )
         {
            cerr << "Error: face " << faceIndex << " is degenerate (fewer than three vertices)!" << endl;
            degenerateFaces = true;
            continue;
         }

         // create a new face
         FaceIter newFace = mesh.faces.insert( mesh.faces.end(), Face());

         // create a new half edge for each edge of the current face
         vector< HalfEdgeIter > hes( N );
         for( int i = 0; i < N; i++ )
         {
            hes[ i ] = mesh.halfedges.insert( mesh.halfedges.end(), HalfEdge());
         }
   
         // initialize these new halfedges
         for( int i = 0; i < N; i++ )
         {
            // the current halfedge goes from vertex a to vertex b
            int a = (*f)[     i     ].position;
            int b = (*f)[ (i+1) % N ].position;
   
            // set current halfedge's attributes
            hes[ i ]->next = hes[ (i+1) % N ];
            hes[ i ]->vertex = indexToVertex[ a ];
            int t = (*f)[i].texcoord;
            if( t >= 0 ) hes[ i ]->texcoord = data.texcoords[ t ];
            else         hes[ i ]->texcoord = Vector( 0., 0., 0. );
            hes[ i ]->onBoundary = false;
   
            // keep track of which halfedges have flip edges defined (for detecting boundaries)
            hasFlipEdge[ hes[ i ]] = false;
   
            // point vertex a at the current halfedge
            indexToVertex[ a ]->he = hes[ i ];
   
            // point the new face and this half edge to each-other
            hes[ i ]->face = newFace;
            newFace->he = hes[ i ];
   
            // if we've created an edge between a and b in the past, it is the
            // flip edge of the current halfedge
            if( a > b ) swap( a, b );
            if( existingHalfEdges.find( pair<int,int>( a, b )) != existingHalfEdges.end())
            {
               hes[ i ]->flip = existingHalfEdges[ pair<int,int>( a, b ) ];
               hes[ i ]->flip->flip = hes[ i ];
               hes[ i ]->edge = hes[ i ]->flip->edge;
               hasFlipEdge[ hes[ i ]] = true;
               hasFlipEdge[ hes[ i ]->flip ] = true;
            }
            else // otherwise, create an edge connected to the current halfedge
            {
               hes[ i ]->edge = mesh.edges.insert( mesh.edges.end(), Edge());
               hes[ i ]->edge->he = hes[i];
               edgeCount[ pair<int,int>( a, b ) ] = 0;
            }
   
            // record the fact that we've created a halfedge from a to b
            existingHalfEdges[ pair<int,int>( a, b ) ] = hes[ i ];

            // check for nonmanifold edges
            edgeCount[ pair<int,int>( a, b ) ]++;
            if( edgeCount[ pair<int,int>( a, b ) ] > 2 )
            {
               cerr << "Error: edge (" << a << ", " << b << ") is nonmanifold (more than two faces sharing a single edge)!" << endl;
               return 1;
            }
         }

         faceIndex++;
      }

      // give up now if there were degenerate faces
      if( degenerateFaces )
      {
         return 1;
      }
   
      // insert extra faces for each boundary cycle
      for( HalfEdgeIter currentHE  = mesh.halfedges.begin();
                        currentHE != mesh.halfedges.end();
                        currentHE ++ )
      {
         // if we find a halfedge with no flip edge defined, create
         // a new face and link it to the corresponding boundary cycle
   
         if( !hasFlipEdge[ currentHE ] )
         {
            // create a new face
            FaceIter newBoundary = mesh.boundaries.insert( mesh.boundaries.end(), Face());
   
            // walk along this boundary cycle
            vector<HalfEdgeIter> boundaryCycle;
            HalfEdgeIter he = currentHE;
            do
            {
               // create a new halfedge on the boundary face
               HalfEdgeIter newHE = mesh.halfedges.insert( mesh.halfedges.end(), HalfEdge());
   
               // mark only the halfedge on the boundary face as being on the boundary
               newHE->onBoundary = true;
   
               // link the current halfedge in the cycle to its new flip edge
               he->flip = newHE;
   
               // grab the next halfedge along the boundary by finding
               // the next halfedge around the current vertex that doesn't
               // have a flip edge defined
               HalfEdgeIter nextHE = he->next;
               while( hasFlipEdge[ nextHE ] )
               {
                  nextHE = nextHE->flip->next;
               }
   
               // set attributes for the flip edge (we'll set ->next below)
               newHE->flip = he;
               newHE->vertex = nextHE->vertex;
               newHE->edge = he->edge;
               newHE->face = newBoundary;
               newHE->texcoord = nextHE->texcoord;
   
               // point the new face to this half edge
               newBoundary->he = newHE;
   
               // keep track of all the new halfedges in the boundary cycle
               boundaryCycle.push_back( newHE );
   
               // continue to walk along the cycle
               he = nextHE;
   
            } while( he != currentHE );
   
            // link together the cycle of boundary halfedges
            unsigned int N = boundaryCycle.size();
            for( unsigned int i = 0; i < N; i++ )
            {
               boundaryCycle[ i ]->next = boundaryCycle[ (i+N-1)%N ];
               hasFlipEdge[ boundaryCycle[i] ] = true;
               hasFlipEdge[ boundaryCycle[i]->flip ] = true;
            }
         }
      }

      // print a warning if the input has any non-terminal defects
      checkIsolatedVertices( mesh );
      checkNonManifoldVertices( mesh );

      return 0;
   }
   
   void MeshIO :: readPosition( stringstream& ss, MeshData& data )
   {
      double x, y, z;
   
      ss >> x >> y >> z;
   
      data.positions.push_back( Vector( x, y, z ));
   }
   
   void MeshIO :: readTexCoord( stringstream& ss, MeshData& data )
   {
      double u, v;
   
      ss >> u >> v;
   
      data.texcoords.push_back( Vector( u, v, 0. ));
   }
   
   void MeshIO :: readNormal( stringstream& ss, MeshData& data )
   {
      double x, y, z;
   
      ss >> x >> y >> z;
   
      data.normals.push_back( Vector( x, y, z ));
   }
   
   void MeshIO :: readFace( stringstream& ss, MeshData &data )
   {
      vector<Index> faceIndices;
      string token;
   
      while( ss >> token )
      {
         faceIndices.push_back( parseFaceIndex( token ));
      }
   
      data.indices.push_back( faceIndices );
   }
   
   Index MeshIO :: parseFaceIndex( const string& token )
   {
      // parse indices of the form
      //
      // p/[t]/[n]
      //
      // where p is an index into positions, t is an index into
      // texcoords, n is an index into normals, and [.] indicates
      // that an index is optional
      
      stringstream in( token );
      string indexstring;
      int indices[3] = { -1, -1, -1 };
      int i = 0;
   
      while( getline( in, indexstring, '/' ))
      {
         stringstream ss( indexstring );
         ss >> indices[i++];
      }
   
      // decrement since indices in OBJ files are 1-based
      return Index( indices[0]-1,
                    indices[1]-1,
                    indices[2]-1 );
   }

   void MeshIO :: checkIsolatedVertices( const Mesh& mesh )
   {
      // print a warning if the mesh has any isolated vertices
      int vertexIndex = 0;
      for( VertexCIter v  = mesh.vertices.begin();
                       v != mesh.vertices.end();
                       v ++ )
      {
         if( v->isIsolated() )
         {
            cerr << "Warning: vertex " << vertexIndex << " is isolated (not contained in any face)." << endl;
         }

         vertexIndex++;
      }
   }

   void MeshIO :: checkNonManifoldVertices( const Mesh& mesh )
   {
      map<VertexCIter,int> nIncidentFaces;

      for( FaceCIter f  = mesh.faces.begin();
                     f != mesh.faces.end();
                     f ++ )
      {
         HalfEdgeCIter he = f->he;
         do
         {
            nIncidentFaces[he->vertex]++;
            he = he->next;
         }
         while( he != f->he );
      }

      for( FaceCIter f  = mesh.boundaries.begin();
                     f != mesh.boundaries.end();
                     f ++ )
      {
         HalfEdgeCIter he = f->he;
         do
         {
            nIncidentFaces[he->vertex]++;
            he = he->next;
         }
         while( he != f->he );
      }

      int vertexIndex = 0;
      for( VertexCIter v  = mesh.vertices.begin();
                       v != mesh.vertices.end();
                       v ++ )
      {
         if( nIncidentFaces[v] != v->valence() )
         {
            cerr << "Warning: vertex " << vertexIndex << " is nonmanifold." << endl;
         }

         vertexIndex++;
      }
   }
}

