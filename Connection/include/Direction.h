#ifndef DDG_DIRECTION_H
#define DDG_DIRECTION_H

#include "Mesh.h"
#include "Quaternion.h"

namespace DDG
{
   class DirectionField
   {
   public:
      void generate(Mesh& mesh, double angle, bool debugMode = false)
      {
         setFaceTag(mesh, false);
         
         FaceIter f = mesh.faces.begin();
         init(f, angle);

         HalfEdgeIter he = f->he;
         do
         {
            propagate( he->flip, f->vector );
            he = he->next;
         }
         while( he != f->he );

         if( debugMode ) debug(mesh);
      }
      
   protected:
      void setFaceTag(Mesh& mesh, bool flag)
      {
         for( FaceIter f = mesh.faces.begin();
             f != mesh.faces.end();
             f ++)
         {
            f->tag = flag;
         }
      }
      
      void init(FaceIter f, double angle)
      {
         VertexIter v0 = f->he->vertex;
         VertexIter v1 = f->he->next->vertex;
         Vector v = (v1->position - v0->position).unit();
         
         f->vector = rotate(v, f->normal(), angle);
         f->tag = true;         
      }
      
      void propagate(HalfEdgeIter h, const Vector& v)
      {
         if( h->onBoundary ) return;
         if( h->face->tag ) return;
         
         FaceIter f = h->face;
         f->vector = transport(v, h);
         f->tag = true;
         
         HalfEdgeIter he = f->he;
         do
         {
            propagate( he->flip, f->vector );
            he = he->next;
         }
         while( he != f->he );
      }

      Vector transport(const Vector& v, HalfEdgeIter h) const
      {
         if( h->onBoundary or h->flip->onBoundary ) return v;
         
         Vector p1 = h->flip->vertex->position;
         Vector p0 = h->vertex->position;
         Vector e = (p1 - p0).unit();

         Vector nR = h->flip->face->normal();
         Vector nL = h->face->normal();
         Vector nLxnR = cross( nL, nR );
         double dihedral = atan2( nLxnR.norm(), dot( nL, nR ) );
         if( dot( e, nLxnR ) < 0.0 ) dihedral = -dihedral;
                  
         Vector u = rotate( v, nR, connection(h) );
         return rotate( u, e, dihedral );
      }

      Vector rotate(const Vector& v, const Vector& axis, double angle) const
      {
         angle *= 0.5;
         Quaternion q( std::cos(angle), std::sin(angle) * axis );
         return ( q.conj() * Quaternion(v) * q ).im();
      }
      
      double connection(HalfEdgeIter h) const
      {
         double star1 = 0.5 * ( h->cotan() + h->flip->cotan() );
         double u1 = h->flip->vertex->potential;
         double u0 = h->vertex->potential;
         double angle = star1*(u1 - u0);
         
         // TODO: add harmonic
         
         return angle;
      }
      
      // debug //
      
      void debug(const Mesh& mesh) const
      {
         for( VertexCIter v = mesh.vertices.begin();
             v != mesh.vertices.end();
             v++)
         {
            if( v->onBoundary() ) continue;
            
            Vector u0 = v->he->flip->face->vector;
            Vector u = circulate(v, u0);
            double offset = atan2( cross(u,u0).norm(), dot(u,u0) );

            double sing = 2.*M_PI*v->singularity;
            while( sing <= 0.0     ) sing += 2.*M_PI;
            while( sing >= 2.*M_PI ) sing -= 2.*M_PI;
            if( sing >= M_PI ) sing = 2.*M_PI - sing;
            
            if( std::abs(offset-sing) > 1.0e-8 )
            {
               std::cout << "Vertex" << v->index << ": "
               << "tag = " << v->tag << " ; "
               << "offset = " << (180./M_PI)*offset << " ; "
               << "sing = " << (180./M_PI)*sing
               << std::endl;
            }
         }
      }
      
      Vector circulate(VertexCIter vertex, Vector u) const
      {
         HalfEdgeIter h = vertex->he;
         do
         {
            u = transport(u, h);
            h = h->next->next->flip;
         }
         while( h != vertex->he );
         return u;
      }
   };
}

#endif
