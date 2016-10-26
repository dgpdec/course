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

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GLES3/gl3.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#endif

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
      
      static double angle;
      static double increment;
      // initial angle and increment
      
      static unsigned oldest;
      static unsigned counter;
      static std::vector<int> indices;
      static std::vector<unsigned> age;
      
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
      static void mIncreaseAngle( void );
      static void mDecreaseAngle( void );
      static void mVectorField( void );
      static void mGenerators( void );
      static void mSelected( void );
      
      static void mIncreaseIndexG( void );
      static void mDecreaseIndexG( void );
      static void mIncreaseIndexV( int i );
      static void mDecreaseIndexV( int i );
      
      static void computeOldest( void );
      static void normalizeSingularities( void );
      static void updateIndex( int i, bool increment );
      
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
         menuScreenshot,
         menuIncreaseAngle,
         menuDecreaseAngle,
         menuVectorField,
         menuGenerators,
         menuSelected,
         menuIncreaseIndex0,
         menuDecreaseIndex0,
         menuIncreaseIndex1,
         menuDecreaseIndex1,
         menuIncreaseIndex2,
         menuDecreaseIndex2,
         menuIncreaseIndex3,
         menuDecreaseIndex3,
         menuIncreaseIndex4,
         menuDecreaseIndex4,
         menuIncreaseIndex5,
         menuDecreaseIndex5
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
      static void drawVectorField( void );
      static void drawVertices( void );
      static void drawSelectedVertices( void );
      static void drawIsolatedVertices( void );
      static void pickVertex(int x, int y);
      static void drawGenerators( void );
      
      static void storeViewerState( void );
      static void restoreViewerState( void );
      static int windowSize[2];
      
      static bool renderWireframe;
      // draw wireframe
      
      static bool renderVectorField;
      // draw vector field
      
      static bool renderGenerators;
      // draw generators
      
      static bool renderSelected;
      // draw picked vertices
      
      static Camera camera;
      // keeps track of view state
      
      static GLuint surfaceDL;
      // display list for mesh
      
      static Shader shader;
      // shader used to determine appearance of surface
   };
}

#endif

