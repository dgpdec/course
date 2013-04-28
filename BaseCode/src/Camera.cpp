#include <cmath>
#include <ctime>
#include <algorithm>
using namespace std;

#include "Camera.h"

namespace DDG
{
   Camera :: Camera( void )
   :   pClick( 1. ),
        pDrag( 1. ),
        pLast( 1. ),
        rLast( 1. ),
     momentum( 1. ),
         zoom( 1. )
   {}
   
   Quaternion Camera :: clickToSphere( int x, int y )
   {
      GLint viewport[4];
      glGetIntegerv( GL_VIEWPORT, viewport );
      int w = viewport[2];
      int h = viewport[3];
   
      Quaternion p( 0.,
                    2. * (double) x / (double) w - 1.,
                    2. * (double) y / (double) h - 1.,
                    0. );
   
      if( p.norm2() > 1. )
      {
         p.normalize();
         p.im().z = 0.;
      }
      else
      {
         p.im().z = sqrt( 1. - p.norm2() );
      }
   
      return p;
   }

   Quaternion Camera :: currentRotation( void ) const
   {
      return ( pDrag * pClick.conj() ) * rLast;
   }

   void Camera :: setView( void ) const
   {
      Quaternion r = ( pDrag * pClick.conj() ) * rLast;

      double w = r[0];
      double x = r[1];
      double y = r[2];
      double z = r[3];

      GLdouble M[16] = {
         1.-2.*y*y-2.*z*z, 2.*x*y+2.*w*z, 2.*x*z-2.*w*y, 0.,
         2.*x*y-2.*w*z, 1.-2.*x*x-2.*z*z, 2.*y*z+2.*w*x, 0.,
         2.*x*z+2.*w*y, 2.*y*z-2.*w*x, 1.-2.*x*x-2.*y*y, 0.,
         0., 0., 0., 1.
      };

      glMatrixMode( GL_MODELVIEW );
      glMultMatrixd( M );
   }

   void Camera :: mouse( int button, int state, int x, int y )
   {
      if( state == GLUT_DOWN )
      {
         pClick = pDrag = pLast = clickToSphere( x, y );
         momentum = 1.;
      }
      if( state == GLUT_UP )
      {
         double timeSinceDrag = ( clock() - tLast ) / (double) CLOCKS_PER_SEC;

         if( timeSinceDrag < .1 )
         {
            momentum = pDrag * pLast.conj();
            momentum = ( .03 * momentum + .97 ).unit();
         }
         else
         {
            momentum = 1.;
         }

         rLast = pDrag * pClick.conj() * rLast;
         pClick = pDrag = 1.;
      }
   }

   void Camera :: motion( int x, int y )
   {
      tLast = clock();
      pLast = pDrag;
      pDrag = clickToSphere( x, y );
   }

   void Camera :: idle( void )
   {
      // get time since last idle event
      static int t0 = clock();
      int t1 = clock();
      double dt = (t1-t0) / (double) CLOCKS_PER_SEC;
   
      rLast = momentum * rLast;
      momentum = ( (1.-.5*dt) * momentum + .5*dt ).unit();

      zoom += vZoom*dt;
      vZoom *= max( 0., 1.-5.*dt );
   
      t0 = t1;
   }

   void Camera :: zoomIn( void )
   {
      vZoom -= 0.5;
   }

   void Camera :: zoomOut( void )
   {
      vZoom += 0.5;
   }
}

