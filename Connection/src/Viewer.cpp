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
#include "Direction.h"

#define AGE_SIZE 5

namespace DDG
{
   // declare static member variables
   Mesh Viewer::mesh;
   
   GLuint Viewer::surfaceDL = 0;
   int Viewer::windowSize[2] = { 512, 512 };
   Camera Viewer::camera;
   Shader Viewer::shader;
   
   bool Viewer::renderWireframe = true;
   bool Viewer::renderVectorField = true;
   bool Viewer::renderGenerators = true;
   bool Viewer::renderSelected = true;
   
   double Viewer::angle = 0.0;
   double Viewer::increment = 0.1;
   
   unsigned Viewer::oldest = 0;
   unsigned Viewer::counter = 0;
   std::vector<int> Viewer::indices;
   std::vector<unsigned> Viewer::age = std::vector<unsigned>(AGE_SIZE, 0);
   
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
      glutAddMenuEntry( "[f] Wireframe",  menuWireframe     );
      glutAddMenuEntry( "[↑] Zoom In",    menuZoomIn        );
      glutAddMenuEntry( "[↓] Zoom Out",   menuZoomOut       );
      glutAddMenuEntry( "[-] DecreaseAngle", menuDecreaseAngle );
      glutAddMenuEntry( "[=] IncreaseAngle", menuIncreaseAngle );
      glutAddMenuEntry( "[v] VectorField",  menuVectorField );
      glutAddMenuEntry( "[g] Generators",  menuGenerators );
      glutAddMenuEntry( "[s] Selected",  menuSelected );
      glutAddMenuEntry( "[0] IncreaseIndex0",  menuIncreaseIndex0 );
      glutAddMenuEntry( "[)] DecreaseIndex0",  menuDecreaseIndex0 );
      glutAddMenuEntry( "[1] IncreaseIndex1",  menuIncreaseIndex1 );
      glutAddMenuEntry( "[!] DecreaseIndex1",  menuDecreaseIndex1 );
      glutAddMenuEntry( "[2] IncreaseIndex2",  menuIncreaseIndex2 );
      glutAddMenuEntry( "[@] DecreaseIndex2",  menuDecreaseIndex2 );
      glutAddMenuEntry( "[3] IncreaseIndex3",  menuIncreaseIndex3 );
      glutAddMenuEntry( "[#] DecreaseIndex3",  menuDecreaseIndex3 );
      glutAddMenuEntry( "[4] IncreaseIndex4",  menuIncreaseIndex4 );
      glutAddMenuEntry( "[$] DecreaseIndex4",  menuDecreaseIndex4 );
      glutAddMenuEntry( "[5] IncreaseIndex5",  menuIncreaseIndex5 );
      glutAddMenuEntry( "[%] DecreaseIndex5",  menuDecreaseIndex5 );
      
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
         case( menuIncreaseAngle ):
            mIncreaseAngle();
            break;
         case( menuDecreaseAngle ):
            mDecreaseAngle();
            break;
         case( menuIncreaseIndex0 ):
            mIncreaseIndexG();
            break;
         case( menuDecreaseIndex0 ):
            mDecreaseIndexG();
            break;
         case( menuIncreaseIndex1 ):
            mIncreaseIndexV(1);
            break;
         case( menuDecreaseIndex1 ):
            mDecreaseIndexV(1);
            break;
         case( menuIncreaseIndex2 ):
            mIncreaseIndexV(2);
            break;
         case( menuDecreaseIndex2 ):
            mDecreaseIndexV(2);
            break;
         case( menuIncreaseIndex3 ):
            mIncreaseIndexV(3);
            break;
         case( menuDecreaseIndex3 ):
            mDecreaseIndexV(3);
            break;
         case( menuIncreaseIndex4 ):
            mIncreaseIndexV(4);
            break;
         case( menuDecreaseIndex4 ):
            mDecreaseIndexV(4);
            break;
         case( menuIncreaseIndex5 ):
            mIncreaseIndexV(5);
            break;
         case( menuDecreaseIndex5 ):
            mDecreaseIndexV(5);
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
         case( menuVectorField ):
            mVectorField();
            break;
         case( menuGenerators ):
            mGenerators();
            break;
         case( menuSelected ):
            mSelected();
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
            mProcess();
            break;
         case 27:
            mExit();
            break;
         case '-':
            mDecreaseAngle();
            break;
         case '=':
            mIncreaseAngle();
            break;
         case 'v':
            mVectorField();
            break;
         case 'g':
            mGenerators();
            break;
         case 's':
            mSelected();
            break;
         case '0':
            mIncreaseIndexG();
            break;
         case ')':
            mDecreaseIndexG();
            break;
         case '1':
            mIncreaseIndexV(0);
            break;
         case '!':
            mDecreaseIndexV(0);
            break;
         case '2':
            mIncreaseIndexV(1);
            break;
         case '@':
            mDecreaseIndexV(1);
            break;
         case '3':
            mIncreaseIndexV(2);
            break;
         case '#':
            mDecreaseIndexV(2);
            break;
         case '4':
            mIncreaseIndexV(3);
            break;
         case '$':
            mDecreaseIndexV(3);
            break;
         case '5':
            mIncreaseIndexV(4);
            break;
         case '%':
            mDecreaseIndexV(4);
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
      if( ( glutGetModifiers() and GLUT_ACTIVE_SHIFT) and state == GLUT_UP )
         pickVertex(x, y);
      else
         camera.mouse( button, state, x, y );
   }
   
   void Viewer :: motion( int x, int y )
   {
      camera.motion( x, y );
   }
   
   void Viewer :: idle( void )
   {
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
   
   void Viewer :: mIncreaseIndexG( void )
   {
      if( mesh.generators.empty() ) return;
      mesh.firstGeneratorIndex++;
      std::cout << "SingG0: " << mesh.firstGeneratorIndex << std::endl;
   }
   
   void Viewer :: mDecreaseIndexG( void )
   {
      if( mesh.generators.empty() ) return;
      mesh.firstGeneratorIndex--;
      std::cout << "SingG0: " << mesh.firstGeneratorIndex << std::endl;
   }
   
   void Viewer :: mIncreaseIndexV( int i )
   {
      if( int(indices.size()) < i) return;
      updateIndex(i, true);
   }
   
   void Viewer :: mDecreaseIndexV( int i )
   {
      if( int(indices.size()) < i) return;
      updateIndex(i, false);
   }
   
   void Viewer :: updateIndex( int i, bool increment )
   {
      if( increment )
         mesh.vertices[ indices[i] ].singularity++;
      else
         mesh.vertices[ indices[i] ].singularity--;
      
      std::cout << "SingV" << indices[i] << ": "
      << mesh.vertices[ indices[i] ].singularity  << std::endl;
      
      counter++;
      age[i] = counter;
      
      computeOldest();
      normalizeSingularities();
   }
   
   void Viewer :: computeOldest( void )
   {
      if( indices.empty() ) return;
      
      unsigned oldest_age = counter + 1;
      for( unsigned i = 0; i < indices.size(); ++i)
      {
         if( age[i] < oldest_age )
         {
            oldest_age = age[i];
            oldest = i;
         }
      }
   }
   
   void Viewer :: normalizeSingularities( void )
   {
      if( indices.empty() ) return;
      
      double sing = mesh.firstGeneratorIndex;
      for( unsigned i = 0; i < indices.size(); ++i)
         sing += mesh.vertices[ indices[i] ].singularity;
      
      double chi = mesh.getEulerCharacteristicNumber();
      mesh.vertices[ indices[oldest] ].singularity += (chi - sing);
      
      std::cout << "OldestV" << indices[oldest] << ": "
      << mesh.vertices[ indices[oldest] ].singularity << std::endl;
   }
   
   void Viewer :: mIncreaseAngle( void )
   {
      angle += increment;
      DirectionField field;
      field.generate( mesh, angle );
      updateDisplayList();
      std::cout << "Angle = " << angle << std::endl;
   }
   
   void Viewer :: mDecreaseAngle( void )
   {
      angle -= increment;
      DirectionField field;
      field.generate( mesh, angle );
      updateDisplayList();
      std::cout << "Angle = " << angle << std::endl;
   }
   
   void Viewer :: mProcess( void )
   {
      Application app;
      bool ok = app.solveForConneciton(mesh);
      
      if( ok )
      {
         DirectionField field;
         field.generate( mesh, angle );
      }
      
      updateDisplayList();
   }
   
   void Viewer :: mSelected( void )
   {
      renderSelected = !renderSelected;
      updateDisplayList();
   }
   
   void Viewer :: mResetMesh( void )
   {
      mesh.reload();
      mesh.init();
      
      DirectionField field;
      field.generate( mesh, angle );
      
      oldest = 0;
      counter = 0;
      indices.clear();
      age = std::vector<unsigned>(AGE_SIZE,0);
      
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
   
   void Viewer :: mVectorField( void )
   {
      renderVectorField = !renderVectorField;
      updateDisplayList();
   }
   
   void Viewer :: mGenerators( void )
   {
      renderGenerators = !renderGenerators;
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
      
      glEnable( GL_POLYGON_OFFSET_FILL );
      glPolygonOffset( 1., 1. );
      glColor3d( 1., .5, .25 );
      drawPolygons();
      glDisable( GL_POLYGON_OFFSET_FILL );
      
      if( renderWireframe ) drawWireframe();
      
      if( renderVectorField ) drawVectorField();
      
      if( renderGenerators ) drawGenerators();
      
      drawIsolatedVertices();
      
      if( renderSelected ) drawSelectedVertices();
      
      glPopAttrib();
   }
   
   void Viewer :: drawPolygons( void )
   {
      glEnable(GL_COLOR_MATERIAL);
      for( FaceCIter f  = mesh.faces.begin();
          f != mesh.faces.end();
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
      glDisable(GL_COLOR_MATERIAL);
   }
   
   void Viewer :: drawWireframe( void )
   {
      shader.disable();
      glPushAttrib( GL_ALL_ATTRIB_BITS );
      
      glDisable( GL_LIGHTING );
      glColor4f( 0., 0., 0., 0.5 );
      glEnable( GL_BLEND );
      glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      
      glBegin( GL_LINES );
      for( EdgeCIter e  = mesh.edges.begin();
          e != mesh.edges.end();
          e ++ )
      {
         glVertex3dv( &e->he->vertex->position[0] );
         glVertex3dv( &e->he->flip->vertex->position[0] );
      }
      glEnd();
      
      glPopAttrib();
   }
   
   void Viewer :: drawIsolatedVertices( void )
   {
      glPushAttrib( GL_ALL_ATTRIB_BITS );
      
      glPointSize( 5 );
      glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
      glEnable( GL_POINT_SMOOTH );
      glColor3f( 1., 0., 0. );
      
      glBegin( GL_POINTS );
      for( VertexCIter v  = mesh.vertices.begin();
          v != mesh.vertices.end();
          v ++ )
      {
         if( v->isIsolated() )
         {
            glVertex3dv( &v->position[0] );
         }
      }
      glEnd();
      
      glPopAttrib();
   }
   
   void Viewer :: drawVertices( void )
   {
      for( VertexCIter v = mesh.vertices.begin();
          v != mesh.vertices.end();
          v ++ )
      {
         glLoadName(v->index);
         glBegin(GL_POINTS);
         glVertex3dv( &v->position[0] );
         glEnd();
      }
   }
   
   void Viewer :: drawSelectedVertices( void )
   {
      shader.disable();
      glPushAttrib( GL_ALL_ATTRIB_BITS );
      
      glEnable( GL_COLOR_MATERIAL );
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glColor3f( 0., 0.5, 0.5 );
      
      double h = 0.5*mesh.meanEdgeLength();
      for( VertexCIter v = mesh.vertices.begin();
          v != mesh.vertices.end();
          v ++ )
      {
         if( v->tag )
         {
            glPushMatrix();
            glTranslated(v->position.x, v->position.y, v->position.z);
            glutSolidSphere(h, 10, 10);
            glPopMatrix();
         }
      }
      glEnd();
      
      glPopAttrib();
   }
   
   void Viewer :: drawVectorField( void )
   {
      shader.disable();
      glPushAttrib( GL_ALL_ATTRIB_BITS );
      
      double h = 0.25*mesh.meanEdgeLength();
      
      glDisable( GL_LIGHTING );
      glColor3f( 0., 0., 0. );
      glLineWidth( 2.0 );
      
      for( FaceCIter f  = mesh.faces.begin();
          f != mesh.faces.end();
          f ++ )
      {
         if( f->isBoundary() ) continue;
         Vector a = f->barycenter();
         Vector b = a + h*f->vector;
         Vector n = f->normal();
         
         Vector v = b - a;
         Vector v90 = cross(n, v);
         Vector p0 = b;
         Vector p1 = p0 - 0.2 * v - 0.1 * v90;
         Vector p2 = p0 - 0.2 * v + 0.1 * v90;
         
         glBegin( GL_LINES );
         glVertex3dv( &a[0] );
         glVertex3dv( &b[0] );
         glEnd();
         
         glBegin(GL_TRIANGLES);
         glVertex3dv( &p0[0] );
         glVertex3dv( &p1[0] );
         glVertex3dv( &p2[0] );
         glEnd();
      }
      
      glPopAttrib();
   }
   
   void Viewer :: pickVertex(int x, int y)
   {
      int width  = glutGet(GLUT_WINDOW_WIDTH );
      int height = glutGet(GLUT_WINDOW_HEIGHT);
      if( x < 0 || x >= width || y < 0 || y >= height ) return;
      
      int bufSize = mesh.vertices.size();
      GLuint* buf = new GLuint[bufSize];
      glSelectBuffer(bufSize, buf);
      
      GLint viewport[4];
      GLdouble projection[16];
      glGetIntegerv( GL_VIEWPORT, viewport );
      glGetDoublev(GL_PROJECTION_MATRIX, projection);
      
      glRenderMode(GL_SELECT);
      glInitNames();
      glPushName(0);
      
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      gluPickMatrix(x, viewport[3]-y, 10, 10, viewport);
      glMultMatrixd(projection);
      
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      drawVertices();
      glPopMatrix();
      
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      
      glMatrixMode(GL_MODELVIEW);
      long hits = glRenderMode(GL_RENDER);
      
      int index = -1;
      double min_z = 1.0e100;
      for( long i = 0; i < hits; ++i )
      {
         double distance = buf[4*i + 1];
         if( distance < min_z )
         {
            index = buf[4*i + 3];
            min_z = distance;
         }
      }
      delete[] buf;
      
      if (index >= 0)
      {
         if( mesh.vertices[index].tag ) return;
         
         if( indices.size() >= age.size() )
         {
            std::cout << "Reached max number of picked vertices" << std::endl;
            return;
         }
         
         mesh.vertices[index].toggleTag();
         indices.push_back(index);
         mIncreaseIndexV( indices.size()-1 );
         updateDisplayList();
      }
   }
   
   void Viewer :: drawGenerators( void )
   {
      shader.disable();
      glPushAttrib( GL_ALL_ATTRIB_BITS );
      
      glEnable( GL_COLOR_MATERIAL );
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glLineWidth(3.0);
      
      for(unsigned i = 0; i < mesh.generators.size(); ++i)
      {
         if( mesh.isBoundaryGenerator( mesh.generators[i] ) )
            glColor3d(0.0,0.0,0.5);
         else
            glColor3d(0.0,0.5,0.0);
         
         for(unsigned j = 0; j < mesh.generators[i].size(); ++j)
         {
            HalfEdgeIter h = mesh.generators[i][j];
            if( h->onBoundary or h->flip->onBoundary ) continue;
            
            Vector p0 = h->vertex->position;
            Vector p1 = h->flip->vertex->position;
            Vector m = 0.5*(p0 + p1);
            
            Vector f0 = h->face->barycenter();
            Vector f1 = h->flip->face->barycenter();
            
            Vector n0 = h->face->normal();
            Vector n1 = h->flip->face->normal();
            
            glBegin(GL_LINES);
            glNormal3dv( &n0[0] );
            glVertex3dv( &f0[0] );
            glVertex3dv( & m[0] );
            
            glNormal3dv( &n1[0] );
            glVertex3dv( & m[0] );
            glVertex3dv( &f1[0] );
            glEnd();
         }
      }
      
      glPopAttrib();
   }
}

