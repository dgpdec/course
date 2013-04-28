#include <iostream>
using namespace std;

#include "Viewer.h"
#include "DenseMatrix.h"
using namespace DDG;

int main( int argc, char** argv )
{
   if( argc != 3 )
   {
      cerr << "usage: " << argv[0] << " src.obj tgt.obj" << endl;
      return 1;
   }

   Viewer viewer;
   viewer.source.read( argv[1] );
   viewer.target.read( argv[2] );
   viewer.initial.read( argv[1] );
   viewer.mesh.read( argv[1] );
   viewer.init();

   return 0;
}

