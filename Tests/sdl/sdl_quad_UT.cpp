#include "test.hpp"
#include "SDLTest.hpp"

#include <string>
#include <iostream>

#ifndef __linux__
#include <Windows.h>
#endif

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.H>
#include <GL/glu.h>
#include <stdio.h>
#include <string>

using namespace sisu;

namespace
{

class sdl_quad_UT : public context
{
	public:
		sdl_quad_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

class SDLColoredQuad : public SDLTestWindow
{

		int32_t mW, mH; 

		GLuint mProgramID;
	
		GLint mVertexPos2DLocation;

		GLuint mVBO, mIBO;

		public:
			SDLColoredQuad( )
				: SDLTestWindow( )
				, mW( 0 )
				, mH( 0 )
				, mProgramID( 0 )
				, mVertexPos2DLocation( -1 )
				, mVBO( 0 ) 
				, mIBO( 0 )
			{
				;
			}

			~SDLColoredQuad( ) 
			{
				glDeleteProgram( mProgramID );
			}
	
			virtual void render( ) 
			{
				glClear( GL_COLOR_BUFFER_BIT );

				glUseProgram( mProgramID );

				glEnableVertexAttribArray( mVertexPos2DLocation );

				glBindBuffer( GL_ARRAY_BUFFER, mVBO );
				glVertexAttribPointer( mVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL );
	
				glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBO );
				glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

				glDisableVertexAttribArray( mVertexPos2DLocation );

				glUseProgram( 0 );
			}

			virtual void run( )
			{
				for ( int ii = 0; ii < 500; ++ii )
				{
					render( );

					SDL_GL_SwapWindow( mMainWindow );
				}
			}

			virtual void initialize( OpenGLAttributes const & xOpenGLAttributes ) 
			{
				SDLTestWindow::initialize( xOpenGLAttributes );

				_stealContext( );

				SDL_GetWindowSize( mMainWindow, &mW, &mH );
				
				mProgramID = glCreateProgram( );
			
				auto compileShader = []( GLenum const xShaderType, const GLchar * xSource ) -> GLuint
				{
					GLuint const shader = glCreateShader( xShaderType );
	
					glShaderSource( shader, 1, &xSource, NULL );

					glCompileShader( shader );

					GLint shaderCompiled = GL_FALSE;
				
				        glGetShaderiv( shader, GL_COMPILE_STATUS, &shaderCompiled );
			
					if ( shaderCompiled != GL_TRUE ) 
					{
						std::cout << "Failed to compile shader: " << std::endl << xSource << std::endl;

						_printGLShaderLog( shader ); 

						exit( -1 );
					}
	
					return shader;
				}; 

				glAttachShader( mProgramID, compileShader( GL_VERTEX_SHADER, "#version 140\n"
		 						 			     "in vec2 LVertexPos2D;"
										  	     "void main() {"
										  	     " gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 );"
										  	     "}" ) );

				glAttachShader( mProgramID, compileShader( GL_FRAGMENT_SHADER, "#version 140\n"
										    	       "out vec4 LFragment;"
										    	       "void main() {"
								  		    	       " LFragment = vec4( 1.0, 1.0, 1.0, 1.0 );"
								  		    	       "}" ) );

				glLinkProgram( mProgramID );

				GLint programSuccess = GL_FALSE;

			        glGetProgramiv( mProgramID, GL_LINK_STATUS, &programSuccess );

				if ( programSuccess != GL_TRUE )
				{
					std::cerr << "Failed to link shader." << std::endl;

					_printGLProgramLog( mProgramID ); 

					exit( -1 ); 
				}

				// TODO: Automate these procedural things so it can be used like a key.
				// i.e myshaderProgram["LVertexPos2D"] = new value
				mVertexPos2DLocation = glGetAttribLocation( mProgramID, "LVertexPos2D" );
		
		                if( mVertexPos2DLocation == -1 )
                		{
					std::cerr << "GLSL program variable LVertexPOS2D not found." << std::endl;
					exit( -1 );
		                }


				glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

		                GLfloat vertexData[] =
                 		{
		                        -0.5f, -0.5f,
		                         0.5f, -0.5f,
		                         0.5f,  0.5f,
		                        -0.5f,  0.5f
		                };

 		                GLuint indexData[] = { 0, 1, 2, 3 };
 
                		glGenBuffers( 1, &mVBO );
 	 	                glBindBuffer( GL_ARRAY_BUFFER, mVBO );
                		glBufferData( GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW );

                		glGenBuffers( 1, &mIBO );
		                glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBO );
                		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW );

				GLenum const error = glGetError( );
				
				if ( error != GL_NO_ERROR ) 
				{
					std::cerr << "SDLColoredQuad::initialize( ) failed!" << std::endl;
					exit( -1 ); 
				}
			}
		}; // class

} // namespace


TEST(sdl_quad_UT, CreateSDLWindowWithoutExceptions)
{

	{
		SDLColoredQuad test;

		test.initialize( { 3, 1, 1, false, false } );

		test.run( );
	}
}
