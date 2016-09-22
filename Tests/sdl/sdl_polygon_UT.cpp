#if 0
#include "test.hpp"
#include "SDLTest.hpp"
#include "SDLShaderTest.hpp"
#include "ioassist.hpp"

#include <string>
#include <iostream>
#include <functional>

#ifndef __linux__
#include <Windows.h>
#endif

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.H>
#include <GL/glu.h>
#include <stdio.h>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace sisu;

namespace {

class sdl_polygon_UT : public context
{
	public:
		sdl_polygon_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

class SDLPolygonTest : public SDLTestShaderWindow
{
	GLint mPolygonColorLocation
	    , mProjectionMatrixLocation
	    , mModelViewMatrixLocation;
	
	glm::mat4 mProjectionMatrix
		, mModelViewMatrix;
	
	GLuint mVBO, mIBO;

	private:
		void _setColor( GLfloat const xR
			      , GLfloat const xG
			      , GLfloat const xB
			      , GLfloat const xA )
		{
			glUniform4f( mPolygonColorLocation, xR, xG, xB, xA );
		}

		void _setProjection( glm::mat4 const & xMatrix )
		{
			mProjectionMatrix = xMatrix;
		}

		void _setModelView( glm::mat4 const & xMatrix )
		{
			mProjectionMatrix = xMatrix;
		}

		void _updateProjection( )
		{
			glUniformMatrix4fv( mProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr( mProjectionMatrix ) );
		}

		void _updateModelView( )
		{
			glUniformMatrix4fv( mModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr( mModelViewMatrix ) );
		}

	protected:
		virtual void render( ) 
		{
			SDLTestShaderWindow::render( );

			glClear( GL_COLOR_BUFFER_BIT );
		
			glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
			
			(*this)( [&] ( )
			{
				_setModelView( glm::translate<GLfloat>( glm::mat4( 1.0f )
					     , glm::vec3( mW / 2.0f, mH / 2.0f, 0.0f ) ) );

				_updateModelView( );

				_setColor( _randomGLfloat( )
					 , _randomGLfloat( )
					 , _randomGLfloat( )
					 , _randomGLfloat( ) );

				glEnableClientState( GL_VERTEX_ARRAY );

					glBindBuffer( GL_ARRAY_BUFFER, mVBO );
					glVertexPointer( 2, GL_FLOAT, 0, NULL );

					glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBO );
					glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

				glDisableClientState( GL_VERTEX_ARRAY );
			} );
		}
	public:
		SDLPolygonTest( ) 
			: SDLTestShaderWindow( ShaderPathPair( "resources/shaders/LExtPolygonProgram2D.glvs"
							     , "resources/shaders/LExtPolygonProgram2d.glfs" ) )
			, mPolygonColorLocation( 0 )
			, mProjectionMatrixLocation( 0 ) 
			, mModelViewMatrixLocation( 0 )
			, mProjectionMatrix( 0 )
			, mModelViewMatrix( 0 )
			, mVBO( 0 )
			, mIBO( 0 )
		{
			;
		}

		~SDLPolygonTest( )
		{
			// TODO: don't forget glDeleteBuffers ..
		}
	
		virtual void initialize( OpenGLAttributes const & xAttributes ) 
		{
			SDLTestShaderWindow::initialize( xAttributes );
	
			auto initializeUniform = [&]( GLint & xUniform, const char * xName )
			{
				if ( ( xUniform = glGetUniformLocation( mShader.getProgramID( ), xName ) ) == -1 )
				{
					std::cerr << xName << " is not a valid GLSL program variable." << std::endl;
	
					exit( -1 );
				}
			};

			initializeUniform( mPolygonColorLocation,     "LPolygonColor" 	  );
			initializeUniform( mProjectionMatrixLocation, "LProjectionMatrix" );
			initializeUniform( mModelViewMatrixLocation,  "LModelViewMatrix"  );
	
			// Initialize model view projection, alpha blending for textures
			glViewport( 0.0f, 0.0f, mW, mH );
		
			glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
	
			glEnable( GL_TEXTURE_2D );
	
			glEnable( GL_BLEND );
			glDisable( GL_DEPTH_TEST );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			
			(*this)([&]()
			{
				_setProjection( glm::ortho<GLfloat>( 0.0f, mW, mH, 0.0, 1.0, -1.0 ) );
				_updateProjection( );
			
				_setModelView( glm::mat4( ) );
				_updateModelView( );

				std::cout << "Initialized MVP" << std::endl;
			});

			static int32_t const sVerticeCount = 4;

			struct LVertexPos2D 
			{	
				GLfloat x;
				GLfloat y;
			} quadVertices[ sVerticeCount ];

			GLuint indices[ sVerticeCount ];
	
			static GLfloat const spacing = 50.0f;

			quadVertices[ 0 ].x = -spacing;
    			quadVertices[ 0 ].y = -spacing;
	
	    		quadVertices[ 1 ].x =  spacing;
	    		quadVertices[ 1 ].y = -spacing;

	    		quadVertices[ 2 ].x =  spacing;
	    		quadVertices[ 2 ].y =  spacing;
	
	    		quadVertices[ 3 ].x = -spacing;
	    		quadVertices[ 3 ].y =  spacing;

    			//Set rendering indices
	    		indices[ 0 ] = 0;
	    		indices[ 1 ] = 1;
	    		indices[ 2 ] = 2;
	    		indices[ 3 ] = 3;

	    		//Create VBO
	    		glGenBuffers( 1, &mVBO );
	    		glBindBuffer( GL_ARRAY_BUFFER, mVBO );
	    		glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(LVertexPos2D), quadVertices, GL_STATIC_DRAW );

	    		//Create IBO
	    		glGenBuffers( 1, &mIBO );
	    		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBO );
	    		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indices, GL_STATIC_DRAW );
		}

		virtual void run( ) 
		{
			for (int i = 0; i < 500; ++i) 
			{
				render( );
				SDL_GL_SwapWindow( mMainWindow );
			}
		}	
};

} // namespace

TEST(sdl_polygon_UT, CreateSDLWindowWithoutExceptions)
{
	{
		SDLPolygonTest test;

		test.initialize( { 3, 1, 1, false, false } );

		test.run( );
	}
}
#endif
