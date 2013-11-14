#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

#include "Viewer.h"

namespace DDG
{
   Vector HSV(double h, double s, double v)
   {
      double r = 0., g = 0., b = 0.;

      if (s == 0)
      {
         r = v;
         g = v;
         b = v;
      }
      else
      {
         h = (h == 1 ? 0 : h) * 6;

         int i = (int)floor(h);

         double f = h - i;
         double p = v * (1 - s);
         double q = v * (1 - (s * f));
         double t = v * (1 - s * (1 - f));

         switch (i)
         {
            case 0:
               r = v;
               g = t;
               b = p;
               break;

            case 1:
               r = q;
               g = v;
               b = p;
               break;

            case 2:
               r = p;
               g = v;
               b = t;
               break;

            case 3:
               r = p;
               g = q;
               b = v;
               break;

            case 4:
               r = t;
               g = p;
               b = v;
               break;

            case 5:
               r = v;
               g = p;
               b = q;
               break;

            default:
               break;
         }
      }

      return Vector(r, g, b);
   }

   Vector qcColor( double qc )
   // standard color map for quasi-conformal distortion
   {
      // clamp to range [1,1.5]
      qc = max( 1., min( 1.5, qc ));

      // compute color
      return HSV( (2.0-4.0*(qc-1.0))/3.0, .7, 0.65 );
   }

   double quasiConformalDistortion( Vector p1, Vector p2, Vector p3,
                                    Vector q1, Vector q2, Vector q3 )
   // computes the quasi-conformal distortion in a triangle with
   // initial vertices (p1,p2,p3) and current vertices (q1,q2,q3)
   {
      // compute edge vectors
      Vector u1 = p2 - p1;
      Vector u2 = p3 - p1;

      Vector v1 = q2 - q1;
      Vector v2 = q3 - q1;

      // compute orthonormal bases
      Vector e1 = u1 / u1.norm();
      Vector e2 = ( u2 - dot(u2,e1)*e1 );
      e2 = e2 / e2.norm();

      Vector f1 = v1 / v1.norm();
      Vector f2 = ( v2 - dot(v2,f1)*f1 );
      f2 = f2 / f2.norm();

      // project onto bases
      p1 = Vector( 0., 0., 0. );
      p2 = Vector( dot(u1,e1), dot(u1,e2), 0. );
      p3 = Vector( dot(u2,e1), dot(u2,e2), 0. );

      q1 = Vector( 0., 0., 0. );
      q2 = Vector( dot(v1,f1), dot(v1,f2), 0. );
      q3 = Vector( dot(v2,f1), dot(v2,f2), 0. );

      double A = cross(p2-p1,p3-p1).norm();

      Vector Ss = (q1*(p2.y-p3.y)+q2*(p3.y-p1.y)+q3*(p1.y-p2.y))/(2.*A);
      Vector St = (q1*(p3.x-p2.x)+q2*(p1.x-p3.x)+q3*(p2.x-p1.x))/(2.*A);
      double a = dot(Ss,Ss);
      double b = dot(Ss,St);
      double c = dot(St,St);
      double Gamma = sqrt(.5*((a+c)+sqrt(sqr(a-c)+4.*b*b)));
      double gamma = sqrt(.5*((a+c)-sqrt(sqr(a-c)+4.*b*b)));

      if( Gamma < gamma )
      {
         swap( Gamma, gamma );
      }

      return Gamma/gamma;
   }

   double faceQCDistortion( FaceCIter f )
   {
      Vector p1 = f->he->vertex->position;
      Vector p2 = f->he->next->vertex->position;
      Vector p3 = f->he->next->next->vertex->position;

      Vector q1 = f->he->vertex->texture;
      Vector q2 = f->he->next->vertex->texture;
      Vector q3 = f->he->next->next->vertex->texture;

      return quasiConformalDistortion( p1, p2, p3,
                                       q1, q2, q3 );
   }
}
