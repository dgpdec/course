#include <iostream>
using namespace std;

#include "Viewer.h"
#include "Direction.h"
using namespace DDG;

int main( int argc, char** argv )
{
   if( argc != 2 )
   {
      cerr << "usage: " << argv[0] << " in.obj" << endl;
      return 1;
   }

   Viewer viewer;
   viewer.mesh.read( argv[1] );
   viewer.mesh.init();
   
   DirectionField field;
   field.generate( viewer.mesh, viewer.angle );
   
   viewer.init();

   return 0;
}

