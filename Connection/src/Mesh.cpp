#include <map>
#include <fstream>
#include "Mesh.h"
#include "MeshIO.h"
#include "DiscreteExteriorCalculus.h"
#include "Direction.h"
#include "TreeCotree.h"
#include "HarmonicBases.h"
#include "Utility.h"

using namespace std;

namespace DDG
{
   Mesh :: Mesh( void )
   {
      firstGeneratorIndex = 0;
   }
   
   Mesh :: Mesh( const Mesh& mesh )
   {
      *this = mesh;
   }
   
   class  HalfEdgeIterCompare { public: bool operator()( const  HalfEdgeIter& i, const  HalfEdgeIter& j ) const { return &*i < &*j; } };
   class HalfEdgeCIterCompare { public: bool operator()( const HalfEdgeCIter& i, const HalfEdgeCIter& j ) const { return &*i < &*j; } };
   class    VertexIterCompare { public: bool operator()( const    VertexIter& i, const    VertexIter& j ) const { return &*i < &*j; } };
   class   VertexCIterCompare { public: bool operator()( const   VertexCIter& i, const   VertexCIter& j ) const { return &*i < &*j; } };
   class      FaceIterCompare { public: bool operator()( const      FaceIter& i, const      FaceIter& j ) const { return &*i < &*j; } };
   class     FaceCIterCompare { public: bool operator()( const     FaceCIter& i, const     FaceCIter& j ) const { return &*i < &*j; } };
   class      EdgeIterCompare { public: bool operator()( const      EdgeIter& i, const      EdgeIter& j ) const { return &*i < &*j; } };
   class     EdgeCIterCompare { public: bool operator()( const     EdgeCIter& i, const     EdgeCIter& j ) const { return &*i < &*j; } };
   
   const Mesh& Mesh :: operator=( const Mesh& mesh )
   {
      map< HalfEdgeCIter, HalfEdgeIter, HalfEdgeCIterCompare > halfedgeOldToNew;
      map<   VertexCIter,   VertexIter,   VertexCIterCompare >   vertexOldToNew;
      map<     EdgeCIter,     EdgeIter,     EdgeCIterCompare >     edgeOldToNew;
      map<     FaceCIter,     FaceIter,     FaceCIterCompare >     faceOldToNew;
      
      // copy geometry from the original mesh and create a
      // map from pointers in the original mesh to
      // those in the new mesh
      halfedges.clear();
      for( HalfEdgeCIter he = mesh.halfedges.begin(); he != mesh.halfedges.end(); he++ )
         halfedgeOldToNew[ he ] = halfedges.insert( halfedges.end(), *he );
      
      vertices.clear();
      for(   VertexCIter  v =  mesh.vertices.begin();  v !=  mesh.vertices.end();  v++ )
         vertexOldToNew[ v  ] =  vertices.insert(  vertices.end(), *v  );
      
      edges.clear();
      for(     EdgeCIter  e =     mesh.edges.begin();  e !=     mesh.edges.end();  e++ )
         edgeOldToNew[ e  ] =     edges.insert(     edges.end(), *e  );

      faces.clear();
      for(     FaceCIter  f =     mesh.faces.begin();  f !=     mesh.faces.end();  f++ )
         faceOldToNew[ f  ] =     faces.insert(     faces.end(), *f  );
      
      // "search and replace" old pointers with new ones
      for( HalfEdgeIter he = halfedges.begin(); he != halfedges.end(); he++ )
      {
         he->next   = halfedgeOldToNew[ he->next   ];
         he->flip   = halfedgeOldToNew[ he->flip   ];
         he->vertex =   vertexOldToNew[ he->vertex ];
         he->edge   =     edgeOldToNew[ he->edge   ];
         he->face   =     faceOldToNew[ he->face   ];
      }
      
      for( VertexIter v = vertices.begin(); v != vertices.end(); v++ ) v->he = halfedgeOldToNew[ v->he ];
      for(   EdgeIter e =    edges.begin(); e !=    edges.end(); e++ ) e->he = halfedgeOldToNew[ e->he ];
      for(   FaceIter f =    faces.begin(); f !=    faces.end(); f++ ) f->he = halfedgeOldToNew[ f->he ];
      
      return *this;
   }
   
   int Mesh::read( const string& filename )
   {
      inputFilename = filename;
      ifstream in( filename.c_str() );
      
      if( !in.is_open() )
      {
         cerr << "Error reading from mesh file " << filename << endl;
         return 1;
      }
      
      int rval;
      if( !( rval = MeshIO::read( in, *this )))
      {
         indexElements();
         normalize();
      }
      return rval;
   }
   
   int Mesh::write( const string& filename ) const
   // reads a mesh from a Wavefront OBJ file; return value is nonzero
   // only if there was an error
   {
      ofstream out( filename.c_str() );
      
      if( !out.is_open() )
      {
         cerr << "Error writing to mesh file " << filename << endl;
         return 1;
      }
      
      MeshIO::write( out, *this );
      
      return 0;
   }
   
   bool Mesh::reload( void )
   {
      return read( inputFilename );
   }
   
   void Mesh::normalize( void )
   {
      // compute center of mass
      Vector c( 0., 0., 0. );
      for( VertexCIter v = vertices.begin(); v != vertices.end(); v++ )
      {
         c += v->position;
      }
      c /= (double) vertices.size();
      
      // translate to origin
      for( VertexIter v = vertices.begin(); v != vertices.end(); v++ )
      {
         v->position -= c;
      }
      
      // rescale such that the mesh sits inside the unit ball
      double rMax = 0.;
      for( VertexCIter v = vertices.begin(); v != vertices.end(); v++ )
      {
         rMax = max( rMax, v->position.norm() );
      }
      for( VertexIter v = vertices.begin(); v != vertices.end(); v++ )
      {
         v->position /= rMax;
      }
   }
   
   void Mesh::indexElements( void )
   {
      int nV = 0;
      for( VertexIter v = vertices.begin(); v != vertices.end(); v++ )
      {
         v->index = nV;
         nV++;
      }
      
      int nE = 0;
      for( EdgeIter e = edges.begin(); e != edges.end(); e++ )
      {
         e->index = nE;
         nE++;
      }
      
      int nF = 0;
      for( FaceIter f = faces.begin(); f != faces.end(); f++ )
      {
         f->index = nF;
         nF++;
      }
   }
   
   double Mesh::area( void ) const
   {
      double sum = 0.0;
      for( FaceCIter f = faces.begin();
          f != faces.end();
          f ++ )
      {
         sum += f->area();
      }
      return sum;
   }
   
   double Mesh::meanEdgeLength( void  ) const
   {
      double sum = 0;
      for( EdgeCIter e = edges.begin();
          e != edges.end();
          e ++)
      {
         VertexIter v0 = e->he->vertex;
         VertexIter v1 = e->he->flip->vertex;
         sum += (v0->position - v1->position).norm();
      }
      return sum / edges.size();
   }
   
   int Mesh :: getEulerCharacteristicNumber( void ) const
   {
      return ( this->vertices.size() - this->edges.size() + this->faces.size() );
   }

   bool Mesh :: isBoundaryGenerator(const Generator& cycle) const
   {
      if( cycle.size() == 0 ) return false;
      return ( cycle[0]->vertex->onBoundary() or
               cycle[0]->flip->vertex->onBoundary() );
   }

   unsigned Mesh :: numberHarmonicBases() const
   {
      unsigned nb = 0;
      bool skipBoundaryLoop = true;
      for(unsigned i = 0; i < generators.size(); ++i)
      {
         const Generator& cycle = generators[i];
         if( skipBoundaryLoop and isBoundaryGenerator(cycle) )
         {
            skipBoundaryLoop = false;
            continue;
         }
         nb++;
      }
      return nb;
   }

   void Mesh :: faceFrame(HalfEdgeIter h, Vector& a, Vector& b) const
   {
      if( h->onBoundary ) return;
      
      VertexIter v0 = h->vertex;
      VertexIter v1 = h->next->vertex;
      
      a = ( v1->position - v0->position ).unit();
      Vector n = h->face->normal();
      b = cross( n, a );
   }
   
   double Mesh :: parallelTransport(HalfEdgeIter h) const
   {
      if( h->onBoundary or h->flip->onBoundary ) return 0.0;
      
      VertexIter v0 = h->vertex;
      VertexIter v1 = h->next->vertex;
      Vector e = v1->position - v0->position;
      
      Vector aL, bL;
      faceFrame( h->face->he, aL, bL );

      Vector aR, bR;
      faceFrame( h->flip->face->he, aR, bR );

      double deltaL = atan2( dot(e,bL), dot(e,aL) );
      double deltaR = atan2( dot(e,bR), dot(e,aR) );
      return (deltaL - deltaR);
   }
   
   double Mesh :: connectionOneForm(HalfEdgeIter h) const
   {
      double angle = 0.0;
      
      // coclosed term
      double star1 = 0.5 * ( h->cotan() + h->flip->cotan() );
      double u0 = h->flip->vertex->potential;
      double u1 = h->vertex->potential;
      angle += star1*(u1 - u0);
      
      // harmonic term
      for(unsigned k = 0; k < h->harmonicBases.size(); ++k)
         angle += this->harmonicCoefs[k] * h->harmonicBases[k];
      
      return angle;
   }
   
   double Mesh :: vertexHolonomy(VertexIter vertex) const
   {
      double sum = 2.0*M_PI;
      HalfEdgeIter h = vertex->he;
      do
      {
         sum += parallelTransport(h);
         sum += connectionOneForm(h);
         h = h->flip->next;
      }
      while( h != vertex->he );
      return sum;
   }
   
   double Mesh :: generatorHolonomy(const Generator& cycle) const
   {
      double sum = 0.0;
      if( cycle.empty() ) return sum;
      
      for(unsigned k = 0; k < cycle.size(); ++k)
      {
         HalfEdgeIter h = cycle[k];
         sum += parallelTransport(h);
         sum += connectionOneForm(h);
      }
      
      while( sum <  0.0      ) sum += 2.0*M_PI;
      while( sum >= 2.0*M_PI ) sum -= 2.0*M_PI;
      return sum;
   }
   
   void Mesh :: init()
   {
      // Laplacian with Neumann boundary condition
      SparseMatrix<Real> star0, star1, d0, Delta;
      HodgeStar0Form<Real>::build( *this, star0 );
      HodgeStar1Form<Real>::build( *this, star1 );
      ExteriorDerivative0Form<Real>::build( *this, d0 );
      Delta = d0.transpose() * star1 * d0;
      
      // make L positive-definite
      Delta += Real(1.0e-8)*star0;
      
      // pre-factorize
      this->L.build(Delta);
      
      // generators
      int t0 = clock();
      TreeCotree tct;
      tct.build( *this );
      int t1 = clock();
      cout << "[generators] time: " << seconds( t0, t1 ) << "s" << "\n";
      
      unsigned nb = this->numberHarmonicBases();
      if( nb == 0 ) return;

      // harmonic coefs
      this->harmonicCoefs = std::vector<double>( nb, 0.0 );
      
      // harmonic bases
      t0 = clock();
      HarmonicBases bases;
      bases.compute( *this );
      t1 = clock();
      cout << "[harmonic] time: " << seconds( t0, t1 ) << "s" << "\n";      
   }
}
