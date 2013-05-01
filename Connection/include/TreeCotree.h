#ifndef DDG_TREE_COTREE_H
#define DDG_TREE_COTREE_H

#include <queue>
#include "Mesh.h"

namespace DDG
{
   class TreeCotree
   {
   public:
      void build(Mesh& mesh)
      {
         mesh.generators.clear();
         buildPrimalSpanningTree(mesh);
         buildDualSpanningCoTree(mesh);
         buildCycles(mesh);
      }
      
   protected:
      void buildCycles(Mesh& mesh)
      {
         for( EdgeIter e = mesh.edges.begin();
              e != mesh.edges.end();
              e++ )
         {
            if( e->he->onBoundary or e->he->flip->onBoundary ) continue;
            if( inPrimalSpanningTree( e->he ) ) continue;
            if( inDualSpanningTree( e->he ) ) continue;

            // path to root from right face
            Mesh::Generator c1;
            FaceIter f = e->he->flip->face;
            while( f != f->parent )
            {
               c1.push_back( sharedHalfEdge( f, f->parent ));
               f = f->parent;
            }

            // path to root from left face
            Mesh::Generator c2;
            f = e->he->face;
            while( f != f->parent )
            {
               c2.push_back( sharedHalfEdge( f, f->parent ));
               f = f->parent;
            }
            
            // build cycle
            Mesh::Generator g;
            g.push_back( e->he );
            int m = c1.size()-1;
            int n = c2.size()-1;
            while( c1[m] == c2[n] ) { m--; n--; }
            for( int i = 0; i <= m; i++ ) g.push_back( c1[i] );
            for( int i = n; i >= 0; i-- ) g.push_back( c2[i]->flip );
               
            // make sure that boundary loops wind around the boundary
            // in a consistent direction
            if( isDualBoundaryLoop( g ))
            {
               if( g[0]->next->vertex->onBoundary() )
               {
                  // reverse cycle
                  unsigned int n = g.size();
                  for( unsigned int i = 0; i < n; i++ )
                  {
                     g[i] = g[i]->flip;
                  }
                  for( unsigned int i = 0; i < n/2; i++ )
                  {
                     swap( g[i], g[n-1-i] );
                  }
               }
            }
               
            mesh.generators.push_back(g);
         }
      }
      
      void buildPrimalSpanningTree(Mesh& mesh)
      {
         // reset parent
         for( VertexIter v = mesh.vertices.begin();
              v != mesh.vertices.end();
              v++ )
         {
            v->parent = v;
         }
         
         // set root as non-boundary vertex
         VertexIter root = mesh.vertices.begin();
         while( root->onBoundary() ) root++;

         // spanning tree
         std::queue<VertexIter> Q;
         Q.push( root );
         while( !Q.empty() )
         {
            VertexIter v = Q.front();
            Q.pop();
            
            HalfEdgeIter he = v->he;
            do
            {
               VertexIter w = he->flip->vertex;
               if( w->parent == w and w != root and !w->onBoundary() )
               {
                  w->parent = v;
                  Q.push( w );
               }
               he = he->flip->next;
            }
            while( he != v->he );
         }
      }
      
      void buildDualSpanningCoTree(Mesh& mesh)
      {
         // reset parent
         for( FaceIter f = mesh.faces.begin();
              f != mesh.faces.end();
              f++ )
         {
            f->parent = f;
         }
         
         // set root as non-boundary face
         FaceIter root = mesh.faces.begin();
         while( root->isBoundary() ) root++;
         
         // spanning tree
         std::queue<FaceIter> Q;
         Q.push( root );
         while( !Q.empty() )
         {
            FaceIter f = Q.front();
            Q.pop();
            
            HalfEdgeIter he = f->he;
            do
            {
               FaceIter g = he->flip->face;
               if( g->parent == g and g != root and !g->isBoundary() and
                   !inPrimalSpanningTree( he ) )
               {
                  g->parent = f;
                  Q.push( g );
               }
               he = he->next;
            }
            while( he != f->he );
         }
      }
      
      bool inPrimalSpanningTree( HalfEdgeIter he ) const
      {
         VertexIter v = he->vertex;
         VertexIter w = he->flip->vertex;
         return v->parent == w or w->parent == v;
      }
      
      bool inDualSpanningTree( HalfEdgeIter he ) const
      {
         FaceIter f = he->face;
         FaceIter g = he->flip->face;
         return f->parent == g or g->parent == f;
      }

      HalfEdgeIter sharedHalfEdge( FaceIter f, FaceIter g ) const
      {
         HalfEdgeIter he = f->he;
         do
         {
            if( he->flip->face == g ) return he;
            he = he->next;
         }
         while( he != f->he );
         assert( 0 );
      }
      
      bool isDualBoundaryLoop( const Mesh::Generator& cycle ) const
      {
         if( cycle.size() == 0 ) return false;
         return ( cycle[0]->vertex->onBoundary() or
                  cycle[0]->flip->vertex->onBoundary() );
      }
   };
}

#endif
