#include "Shader.h"

#include <fstream>
#include <iostream>
using namespace std;

namespace DDG
{
   Shader::Shader( void )
   // constructor -- shader is initially invalid
   : vertexShader( 0 ),
     fragmentShader( 0 ),
     geometryShader( 0 ),
     program( 0 ),
     linked( false )
   {}

   Shader::~Shader( void )
   {
      if( program ) glDeleteProgram( program );

      if(   vertexShader ) glDeleteShader(   vertexShader );
      if( fragmentShader ) glDeleteShader( fragmentShader );
      if( geometryShader ) glDeleteShader( geometryShader );
   }
   
   void Shader::loadVertex( const char* filename )
   {
      load( GL_VERTEX_SHADER, filename, vertexShader );
   }
   
   void Shader::loadFragment( const char* filename )
   {
      load( GL_FRAGMENT_SHADER, filename, fragmentShader );
   }
   
   void Shader::loadGeometry( const char* filename )
   {
#ifdef GL_GEOMETRY_SHADER_EXT
      load( GL_GEOMETRY_SHADER_EXT, filename, geometryShader );
#else
      cerr << "Error: geometry shaders not supported!" << endl;
#endif
   }
   
   void Shader::enable( void )
   {
      if( !linked )
      {
         glLinkProgram( program );
         linked = true;
      }
   
      glUseProgram( program );
   }
   
   void Shader::disable( void ) const
   {
      glUseProgram( 0 );
   }
   
   Shader::operator GLuint( void ) const
   {
      return program;
   }
   
   void Shader::load( GLenum shaderType, const char* filename, GLuint& shader )
   // read vertex shader from GLSL source file, compile, and attach to program
   {
      string source;
      
      if( !readSource( filename, source ))
      {
         return;
      }

      if( program == 0 )
      {
         program = glCreateProgram();
      }
   
      if( shader != 0 )
      {
         glDetachShader( program, shader );
      }
   
      shader = glCreateShader( shaderType );
      const char* source_c_str = source.c_str();
      glShaderSource( shader, 1, &(source_c_str), NULL );
   
      glCompileShader( shader );
      GLint compileStatus;
      glGetShaderiv( shader, GL_COMPILE_STATUS, &compileStatus );
   
      if( compileStatus == GL_TRUE )
      {
         glAttachShader( program, shader );
         linked = false;
      }
      else
      {
         GLsizei maxLength = 0;
         glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
   
         if( maxLength > 0 )
         {
            GLchar* infoLog = new char[ maxLength ];
            GLsizei length;
   
            glGetShaderInfoLog( shader, maxLength, &length, infoLog );
   
            cerr << "GLSL Error: " << infoLog << endl;
   
            delete[] infoLog;
         }
      }
   }

   bool Shader::readSource( const char* filename, std::string& source )
   // reads GLSL source file into a string
   {
      source = "";

      ifstream in( filename );
      if( !in.is_open() )
      {
         cerr << "Error: could not open shader file ";
         cerr << filename;
         cerr << " for input!" << endl;
         return false;
      }

      string line;
      while( getline( in, line ))
      {
         source += line;
      }

      return true;
   }
}

