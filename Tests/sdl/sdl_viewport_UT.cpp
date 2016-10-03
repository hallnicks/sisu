#include "test.hpp"
#include "SDLTest.hpp"
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

using namespace sisu;

namespace {

class sdl_viewport_UT : public context
{
	public:
		sdl_viewport_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};


enum eViewportMode
{
	eViewportMode_Full        = 0,
	eViewportMode_Half_Center = 1,
	eViewportMode_Half_Top	  = 2,
	eViewportMode_Quad	  = 3,
	eViewportMode_Radar	  = 4,
	eViewportMode_Max	  = 5
};


eViewportMode & operator++( eViewportMode & xX ) 
{
	if ( xX >= eViewportMode_Max )
	{
		xX = eViewportMode_Full;
	}
	else
	{
		xX = static_cast<eViewportMode>( static_cast<uint8_t>( xX ) + 1 );
	}
}

class SDLViewportTest : public SDLTestWindow 
{
	eViewportMode mViewportMode;

	protected:
		virtual void render( ) 
		{
			glClear( GL_COLOR_BUFFER_BIT );

			glLoadIdentity( );
	
			glTranslatef( mW / 2.0f, mH / 2.0f, 0.0f );
			
			auto randomColor = [ ] ( )
					{
					        glColor3f( _randomGLfloat( )
							 , _randomGLfloat( )
							 , _randomGLfloat( ) );
					};

			auto drawQuads = [&]( )
					{
    						glBegin( GL_QUADS );
							randomColor( );
						        glVertex2f( -mW / 2.0f, -mH / 2.0f );
    	 		 	 	 	  	glVertex2f(  mW / 2.0f, -mH / 2.0f );
            						glVertex2f(  mW / 2.0f,  mH / 2.0f );
 					                glVertex2f( -mW / 2.0f,  mH / 2.0f );
				                glEnd( );
					};

			switch ( mViewportMode )
			{
				case eViewportMode_Full:
					glViewport( 0.0f, 0.0f, mW, mH ); 
					drawQuads( ); 
					break;
				case eViewportMode_Half_Center:
					glViewport( mW / 4.0f, mH / 4.0f, mW / 2.0f, mH / 2.0f );
					drawQuads( );
					break;
				case eViewportMode_Half_Top:
					glViewport( mW / 4.0f, mH / 2.0f, mW / 2.0f, mH / 2.0f ); 
					drawQuads( ); 
					break;
				case eViewportMode_Quad:
					glViewport( 0.0f, 0.0f, mW / 2.0f, mH / 2.0f );
					drawQuads( );

					glViewport( mW / 2.0f, 0.0f, mW / 2.0f, mH / 2.0f );
					drawQuads( );
				
					glViewport( 0.0f, mH / 2.0f, mW / 2.0f, mH / 2.0f );
					drawQuads( );
				
					glViewport( mW / 2.0f, mH / 2.0f, mW / 2.0f, mH / 2.0f );
					drawQuads( );
					break;
				case eViewportMode_Radar:
					        //Full size quad
				        glViewport( 0.f, 0.f, mW, mH );
				
				        glBegin( GL_QUADS );
						randomColor( );
				            	glVertex2f( -mW / 8.0f, -mH / 8.0f );
				            	glVertex2f(  mW / 8.0f, -mH / 8.0f );
				            	glVertex2f(  mW / 8.0f,  mH / 8.0f );
				            	glVertex2f( -mW / 8.0f,  mH / 8.0f );

						randomColor( );
				            	glVertex2f( -mW / 16.0f, -mH / 16.0f );
				            	glVertex2f(  mW / 16.0f, -mH / 16.0f );
				            	glVertex2f(  mW / 16.0f,  mH / 16.0f );
				            	glVertex2f( -mW / 16.0f,  mH / 16.0f );
				        glEnd();

				        glViewport( mW / 2.f, mH / 2.f, mW / 2.f, mH / 2.f );
				        glBegin( GL_QUADS );
						randomColor( );
 			 		    	glVertex2f( -mW / 8.0f, -mH / 8.0f );
				            	glVertex2f(  mW / 8.0f, -mH / 8.0f );
				            	glVertex2f(  mW / 8.0f,  mH / 8.0f );
				            	glVertex2f( -mW / 8.0f,  mH / 8.0f );
				            
						randomColor( );
				            	glVertex2f( -mW / 16.0f, -mH / 16.0f );
				            	glVertex2f(  mW / 16.0f, -mH / 16.0f );
				            	glVertex2f(  mW / 16.0f,  mH / 16.0f );
				            	glVertex2f( -mW / 16.0f,  mH / 16.0f );
				        glEnd();
					break;
				default:
					break;
			}

		}
	public:
		SDLViewportTest( ) 
			: SDLTestWindow( ) 
			, mViewportMode( eViewportMode_Full )
		{
			;
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestWindow::initialize( xAttributes );

			glViewport( 0.0f, 0.0f, mW, mH );
			
			glMatrixMode( GL_PROJECTION );
			
			glLoadIdentity( );

			glOrtho( 0.0, mW, mH, 0.0, 1.0, -1.0 );

			glMatrixMode( GL_MODELVIEW );

			glLoadIdentity( );

			glClearColor( 0.25f, 0.0f, 0.0f, 1.0f );
		}

		virtual void run ( ) 
		{
			static uint32_t const sFrames = 500;
			static uint32_t const sFramesTenth = sFrames / 10;

			for ( int32_t i = 0; i < sFrames; ++i )
 			{
				if ( i % sFramesTenth == 0 )
				{
					++mViewportMode; 
				}

				render( );
				SDL_GL_SwapWindow( mMainWindow );
			}
		}
};

} // namespace

TEST(sdl_viewport_UT, CreateSDLWindowWithoutExceptions)
{
	{
		SDLViewportTest test;

		test.initialize( { 3, 1, 1, false, false } );

		test.run( );
	}

}
