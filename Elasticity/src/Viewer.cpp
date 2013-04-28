#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

#include "Viewer.h"
#include "Image.h"
#include "Application.h"

namespace DDG
{
   // declare static member variables
   Mesh Viewer::mesh;
   Mesh Viewer::source;
   Mesh Viewer::target;
   Mesh Viewer::initial;
   GLuint Viewer::surfaceDL = 0;
   int Viewer::windowSize[2] = { 512, 512 };
   Camera Viewer::camera;
   Shader Viewer::shader;
   bool Viewer::renderWireframe = false;
   double Viewer::step = 0.0;
   double Viewer::delta = 0.1;
   bool Viewer::renderInitial = true;
   bool Viewer::animate = false;

   void Viewer :: init( void )
   {
      restoreViewerState();
      initGLUT();
      setGL();
      initGLSL();
      updateDisplayList();
      glutMainLoop();
   }
   
   void Viewer :: initGLUT( void )
   {
      int argc = 0;
      vector< vector<char> > argv(1);
      
      // initialize window
      glutInitWindowSize( windowSize[0], windowSize[1] );
      glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
      glutInit( &argc, (char**)&argv );
      glutCreateWindow( "DDG" );
      
      // specify callbacks
      glutDisplayFunc  ( Viewer::display  );
      glutIdleFunc     ( Viewer::idle     );
      glutKeyboardFunc ( Viewer::keyboard );
      glutSpecialFunc  ( Viewer::special  );
      glutMouseFunc    ( Viewer::mouse    );
      glutMotionFunc   ( Viewer::motion   );
      
      // initialize menus
      int viewMenu = glutCreateMenu( Viewer::view );
      glutSetMenu( viewMenu );
      glutAddMenuEntry( "[f] Wireframe",  menuWireframe    );
      glutAddMenuEntry( "[↑] Zoom In",    menuZoomIn       );
      glutAddMenuEntry( "[↓] Zoom Out",   menuZoomOut      );
      glutAddMenuEntry( "[i] Initial",  menuRenderInitial  );
      glutAddMenuEntry( "[-] DecreaseStep", menuDecreaseStep  );
      glutAddMenuEntry( "[=] IncreaseStep", menuIncreaseStep  );

      int mainMenu = glutCreateMenu( Viewer::menu );
      glutSetMenu( mainMenu );
      glutAddMenuEntry( "[space] Process Mesh", menuProcess    );
      glutAddMenuEntry( "[r] Reset Mesh",       menuResetMesh  );
      glutAddMenuEntry( "[w] Write Mesh",       menuWriteMesh  );
      glutAddMenuEntry( "[\\] Screenshot",      menuScreenshot );
      glutAddMenuEntry( "[esc] Exit",           menuExit       );
      glutAddSubMenu( "View", viewMenu );
      glutAttachMenu( GLUT_RIGHT_BUTTON );
   }
   
   void Viewer :: initGLSL( void )
   {
      shader.loadVertex( "shaders/vertex.glsl" );
      shader.loadFragment( "shaders/fragment.glsl" );
   }

   void Viewer :: menu( int value )
   {
      switch( value )
      {
         case( menuProcess ):
            mProcess();
            break;
         case( menuResetMesh ):
            mResetMesh();
            break;
         case( menuWriteMesh ):
            mWriteMesh();
            break;
         case( menuScreenshot ):
            mScreenshot();
            break;
         case( menuExit ):
            mExit();
            break;
         case( menuIncreaseStep ):
            mIncreaseStep();
            break;
         case( menuDecreaseStep ):
            mDecreaseStep();
            break;
         default:
            break;
      }
   }
   
   void Viewer :: view( int value )
   {
      switch( value )
      {
         case( menuWireframe ):
            mWireframe();
            break;
         case( menuZoomIn ):
            mZoomIn();
            break;
         case( menuZoomOut ):
            mZoomOut();
            break;
         case( menuRenderInitial ):
            mRenderInitial();
            break;
         default:
            break;
      }
   }
   
   void Viewer :: keyboard( unsigned char c, int x, int y )
   {
      switch( c )
      {
         case 'f':
            mWireframe();
            break;
         case 'w':
            mWriteMesh();
            break;
         case 'r':
            mResetMesh();
            break;
         case '\\':
            mScreenshot();
            break;
         case ' ':
            animate = !animate;
            break;
         case 27:
            mExit();
            break;
         case '-':
            mDecreaseStep();
            break;
         case '=':
            mIncreaseStep();
            break;
         case 'i':
            mRenderInitial();
            break;
         default:
            break;
      }
   }
   
   void Viewer :: special( int i, int x, int y )
   {
      switch( i )
      {
         case GLUT_KEY_UP:
            camera.zoomIn();
            break;
         case GLUT_KEY_DOWN:
            camera.zoomOut();
            break;
         case 27:
            mExit();
            break;
         default:
            break;
      }
   }

   void Viewer :: mouse( int button, int state, int x, int y )
   {
      camera.mouse( button, state, x, y );
   }
   
   void Viewer :: motion( int x, int y )
   {
      camera.motion( x, y );
   }
   
   void Viewer :: idle( void )
   {
      if( animate )
      {
         mProcess();
      }

      camera.idle();
      glutPostRedisplay();
   }
   
   void Viewer :: storeViewerState( void )
   {
      ofstream out( ".viewer_state.txt" );
      
      out << camera.rLast[0] << endl;
      out << camera.rLast[1] << endl;
      out << camera.rLast[2] << endl;
      out << camera.rLast[3] << endl;
      
      GLint view[4];
      glGetIntegerv( GL_VIEWPORT, view );
      out << view[2] << endl;
      out << view[3] << endl;
   }
   
   void Viewer :: restoreViewerState( void )
   {
      ifstream in( ".viewer_state.txt" );
      if( !in.is_open() ) return;
      
      in >> camera.rLast[0];
      in >> camera.rLast[1];
      in >> camera.rLast[2];
      in >> camera.rLast[3];
      in >> windowSize[0];
      in >> windowSize[1];
   }

   void Viewer :: mRenderInitial( void )
   {
      renderInitial = !renderInitial;
      updateDisplayList();
   }

   void Viewer :: mIncreaseStep( void )
   {
      step += delta;
      step = std::min(step, 1.0);
      
      Application app;
      app.init(step, source, target, mesh);
      app.init(step, source, target, initial);

      updateDisplayList();
      std::cout << "Step = " << step << std::endl;
   }

   void Viewer :: mDecreaseStep( void )
   {
      step -= delta;
      step = std::max(step, 0.0);
      
      Application app;
      app.init(step, source, target, mesh);
      app.init(step, source, target, initial);

      updateDisplayList();
      std::cout << "Step = " << step << std::endl;
   }

   void Viewer :: mProcess( void )
   {
      Application app;
      app.interpolate(step, source, target, mesh, 1);
      updateDisplayList();
   }
   
   void Viewer :: mResetMesh( void )
   {
      mesh.reload();
      updateDisplayList();
   }
   
   void Viewer :: mWriteMesh( void )
   {
      mesh.write( "out.obj" );
   }
   
   void Viewer :: mExit( void )
   {
      //storeViewerState();
      exit( 0 );
   }
   
   void Viewer :: mWireframe( void )
   {
      renderWireframe = !renderWireframe;
      updateDisplayList();
   }
   
   void Viewer :: mZoomIn( void )
   {
      camera.zoomIn();
   }
   
   void Viewer :: mZoomOut( void )
   {
      camera.zoomOut();
   }
   
   void Viewer :: mScreenshot( void )
   {
      static int index = 0;
      
      // get window width and height
      GLint view[4];
      glGetIntegerv( GL_VIEWPORT, view );
      int w = view[2];
      int h = view[3];
      
      // get pixels
      Image image( w, h );
      glReadPixels( 0, 0, w, h, GL_BGR, GL_FLOAT, &image(0,0) );
      
      stringstream filename;
      filename << "frames/viewer" << setw(8) << setfill( '0' ) << index << ".tga";
      image.write( filename.str().c_str() );
      
      index++;
   }
      
   void Viewer :: display( void )
   {
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      shader.enable();

      glMatrixMode( GL_PROJECTION );
      glLoadIdentity();
      GLint viewport[4];
      glGetIntegerv( GL_VIEWPORT, viewport );
      double aspect = (double) viewport[2] / (double) viewport[3];
      const double fovy = 50.;
      const double clipNear = .01;
      const double clipFar = 1000.;
      gluPerspective( fovy, aspect, clipNear, clipFar );
      
      glMatrixMode( GL_MODELVIEW );
      glLoadIdentity();
      
      Quaternion    eye = Vector( 0., 0., -2.5*camera.zoom );
      Quaternion center = Vector( 0., 0., 0. );
      Quaternion     up = Vector( 0., 1., 0. );      
      gluLookAt(   eye[1],    eye[2],    eye[3],
                center[1], center[2], center[3],
                    up[1],     up[2],     up[3] );
      
      
      Quaternion r = camera.currentRotation();
      eye = r.conj() * eye * r;
      GLint uniformEye = glGetUniformLocation( shader, "eye" );
      glUniform3f( uniformEye, eye[1], eye[2], eye[3] );
      
      Quaternion light = Vector( -1., 1., -2. );
      light = r.conj() * light * r;
      GLint uniformLight = glGetUniformLocation( shader, "light" );
      glUniform3f( uniformLight, light[1], light[2], light[3] );
      
      camera.setView();
      callDisplayList();
      shader.disable();
      glutSwapBuffers();
   }
   
   void Viewer :: updateDisplayList( void )
   {
      if( surfaceDL )
      {
         glDeleteLists( surfaceDL, 1 );
         surfaceDL = 0;
      }
      
      surfaceDL = glGenLists( 1 );
      glNewList( surfaceDL, GL_COMPILE );
      setMeshMaterial();
      drawScene();
      glEndList();
   }
   
   void Viewer :: setGL( void )
   {
      glClearColor( .5, .5, .5, 1. );
      setLighting();
   }
   
   void Viewer :: setLighting( void )
   {
      GLfloat position[4] = { 20., 30., 40., 0. };
      glLightfv( GL_LIGHT0, GL_POSITION, position );
      glEnable( GL_LIGHT0 );
      glEnable( GL_NORMALIZE );
   }
   
   void Viewer :: setMeshMaterial( void )
   {
      GLfloat  diffuse[4] = { .8, .5, .3, 1. };
      GLfloat specular[4] = { .3, .3, .3, 1. };
      GLfloat  ambient[4] = { .2, .2, .5, 1. };

      glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   diffuse  );
      glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  specular );
      glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,   ambient  );
      glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, 16.      );
   }
   
   void Viewer :: callDisplayList( void )
   {
      glPushAttrib( GL_ALL_ATTRIB_BITS );
      glEnable( GL_DEPTH_TEST );
      glEnable( GL_LIGHTING );
      glCallList( surfaceDL );
      glPopAttrib();
   }
   
   void Viewer :: drawScene( void )
   {
      glPushAttrib( GL_ALL_ATTRIB_BITS );
      glEnable(GL_COLOR_MATERIAL);

      if( renderInitial )
      {
         glEnable( GL_POLYGON_OFFSET_FILL );
         glPolygonOffset( 1., 1. );
         glColor3f( 0.5, 0.5, 0. );
         drawPolygons( initial );
         glDisable( GL_POLYGON_OFFSET_FILL );
         if( renderWireframe ) drawWireframe( initial );
      }
      
      glEnable( GL_POLYGON_OFFSET_FILL );
      glPolygonOffset( 5., 1. );
      glColor3f( .8, .5, .3 );
      drawPolygons( mesh );
      glDisable( GL_POLYGON_OFFSET_FILL );
      if( renderWireframe ) drawWireframe( mesh );
      
      glPopAttrib();
   }
   
   void Viewer :: drawPolygons( const Mesh& surf )
   {
      for( FaceCIter f  = surf.faces.begin();
          f != surf.faces.end();
          f ++ )
      {
         if( f->isBoundary() ) continue;
         
         glBegin( GL_POLYGON );
         if( renderWireframe )
         {
            Vector N = f->normal();
            glNormal3dv( &N[0] );
         }
         
         HalfEdgeCIter he = f->he;
         do
         {
            if( not renderWireframe )
            {
               Vector N = he->vertex->normal();
               glNormal3dv( &N[0] );
            }
            
            glVertex3dv( &he->vertex->position[0] );
            
            he = he->next;
         }
         while( he != f->he );
         glEnd();
      }
   }
   
   void Viewer :: drawWireframe( const Mesh& surf )
   {
      shader.disable();
      glPushAttrib( GL_ALL_ATTRIB_BITS );
      
      glDisable( GL_LIGHTING );
      glColor4f( 0., 0., 0., 0.5 );
      glEnable( GL_BLEND );
      glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      
      glBegin( GL_LINES );
      for( EdgeCIter e  = surf.edges.begin();
          e != surf.edges.end();
          e ++ )
      {
         glVertex3dv( &e->he->vertex->position[0] );
         glVertex3dv( &e->he->flip->vertex->position[0] );
      }
      glEnd();
      
      glPopAttrib();
   }
}
