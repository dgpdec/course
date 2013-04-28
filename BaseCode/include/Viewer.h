// -----------------------------------------------------------------------------
// libDDG -- Viewer.h
// -----------------------------------------------------------------------------
//
// Viewer provides a graphical user interface (GUI) for inspecting and
// interacting with a Mesh object.  Viewer methods are static in order
// to make them compatible with GLUT callbacks.
//

#ifndef DDG_VIEWER_H
#define DDG_VIEWER_H

#include <GLUT/glut.h>
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"

namespace DDG
{
   class Viewer
   {
   public:
      static void init( void );
      // displays the viewer until the program ends
      
      static Mesh mesh;
      // surface mesh visualized by Viewer
      
   protected:
      // init
      static void initGLUT( void );
      static void initGLSL( void );

      // GLUT callbacks
      static void display( void );
      static void idle( void );
      static void keyboard( unsigned char c, int x, int y );
      static void special( int i, int x, int y );
      static void mouse( int button, int state, int x, int y );
      static void motion( int x, int y );
      static void menu( int value );
      static void view( int value );
      
      // menu functions
      static void mProcess( void );
      static void mResetMesh( void );
      static void mWriteMesh( void );
      static void mExit( void );
      static void mWireframe( void );
      static void mZoomIn( void );
      static void mZoomOut( void );
      static void mScreenshot( void );
      
      // unique identifiers for menus
      enum
      {
         menuProcess,
         menuResetMesh,
         menuWriteMesh,
         menuExit,
         menuWireframe,
         menuZoomIn,
         menuZoomOut,
         menuScreenshot
      };
      
      // draw routines
      static void setGL( void );
      static void setLighting( void );
      static void setMeshMaterial( void );
      static void callDisplayList( void );
      static void updateDisplayList( void );
      static void drawScene( void );
      static void drawPolygons( void );
      static void drawWireframe( void );
      static void drawVertices( void );
      static void drawSelectedVertices( void );
      static void drawIsolatedVertices( void );
      static void pickVertex(int x, int y);
      
      static void storeViewerState( void );
      static void restoreViewerState( void );
      static int windowSize[2];
      
      static bool renderWireframe;
      // draw wireframe
      
      static Camera camera;
      // keeps track of view state
      
      static GLuint surfaceDL;
      // display list for mesh
      
      static Shader shader;
      // shader used to determine appearance of surface
   };
}

#endif

