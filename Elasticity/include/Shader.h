// -----------------------------------------------------------------------------
// libDDG -- Shader.h
// -----------------------------------------------------------------------------
//
// Shader encapsulates the functionality of a shader program written in
// the OpenGL Shader Language (GLSL).  Basic usage is to read a collection
// of source files to disk and enable the shader before making draw calls.
// For instance, during initialization one might write
//
//    Shader shader;
//    shader.loadVertex( "vertex.glsl" );
//    shader.loadFragment( "fragment.glsl" );
//
// and in the main draw routine write
//
//    shader.enable();
//    // draw some stuff
//    shader.disable();
// 

#ifndef DDG_SHADER_H
#define DDG_SHADER_H

#include <GLUT/glut.h>
#include <string>

namespace DDG
{
   class Shader
   {
      public:
         Shader( void );
         // constructor -- shader is initially invalid

         ~Shader( void );
         // destructor

         void loadVertex( const char* filename );
         // read vertex shader from GLSL source file

         void loadFragment( const char* filename );
         // read fragment shader from GLSL source file

         void loadGeometry( const char* filename );
         // read geometry shader from GLSL source file

         void enable( void );
         // uses this shader for rendering

         void disable( void ) const;
         // uses the fixed-function pipeline for rendering

         operator GLuint( void ) const;
         // returns the ID of this shader program (for calls to OpenGL)

      protected:
         void load( GLenum shaderType, const char* filename, GLuint& shader );
         bool readSource( const char* filename, std::string& source );

         GLuint vertexShader;
         GLuint fragmentShader;
         GLuint geometryShader;
         GLuint program;
         bool linked;
   };
}

#endif
