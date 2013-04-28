// -----------------------------------------------------------------------------
// libDDG -- Image.h
// -----------------------------------------------------------------------------
//
// Image represents a color bitmap image.  A simple example might look like
// 
//    Image im;
//    im.read( "input.tga" );
//    // modify image data via im(x,y) = ...;
//    im.write( "output.tga" );
// 

#ifndef DDG_IMAGE_H
#define DDG_IMAGE_H

#include <vector>
#include <string>

namespace DDG
{
   class Image
   {
      public:
         Image( int width = 0, int height = 0 );
         // constructs image with specified width and height

         float& operator()( int x, int y );
         const float& operator()( int x, int y ) const;
         // accesses pixel (x,y)

         float sample( float x, float y ) const;
         // samples image at (x,y) using bilinear filtering

         int  width( void ) const;
         int height( void ) const;
         // returns image dimensions

         void read( const char* filename );
         // loads an image file in Truevision TGA format
         // (must be RGB image with 24 or 32 bits per pixel)

         void write( const char* filename ) const;
         // writes an image file in Truevision TGA format
         // (RGB image with 24 bits per pixel)

      protected:
         void clamp( int& x, int& y ) const;
         // clamps coordinates to range [0,w-1] x [0,h-1]

         int w, h;
         // width and height

         std::vector<float> pixels;
         // interleaved RGBA pixel data in range [0-1]
   };
}

#endif
