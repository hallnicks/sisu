#ifdef SDL_FBO_UT
#include "test.hpp"
#include "SDLTest.hpp"
#include "threadgears.hpp"

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

#include "gltools.hpp"
#include "teapot.h"

using namespace sisu;

namespace {

class sdl_FBO_UT : public context
{
	public:
		sdl_FBO_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};


class FBOTestWindow : public SDLTestWindow
{
	GLuint mFBOID
	     , mTextureID
	     , mRBOID;

	bool mMouseLeftDown
	   , mMouseRightDown;

	float mMouseX
	    , mMouseY
	    , mCameraAngleX
	    , mCameraAngleY
	    , mCameraDistance
            , mPlayTime;


	void _drawTexturedCube( )
	{
		// TODO: Use Texture2D object.
		glBindTexture(GL_TEXTURE_2D, mTextureID);

		glColor4f(1, 1, 1, 1);
		glBegin(GL_TRIANGLES);
        		// front faces
        		glNormal3f(0,0,1);
		        // face v0-v1-v2
		        glTexCoord2f(1,1);  glVertex3f(1,1,1);
		        glTexCoord2f(0,1);  glVertex3f(-1,1,1);
		        glTexCoord2f(0,0);  glVertex3f(-1,-1,1);
		        // face v2-v3-v0
		        glTexCoord2f(0,0);  glVertex3f(-1,-1,1);
		        glTexCoord2f(1,0);  glVertex3f(1,-1,1);
		        glTexCoord2f(1,1);  glVertex3f(1,1,1);

		        // right faces
		        glNormal3f(1,0,0);
		        // face v0-v3-v4
		        glTexCoord2f(0,1);  glVertex3f(1,1,1);
		        glTexCoord2f(0,0);  glVertex3f(1,-1,1);
		        glTexCoord2f(1,0);  glVertex3f(1,-1,-1);
		        // face v4-v5-v0
		        glTexCoord2f(1,0);  glVertex3f(1,-1,-1);
		        glTexCoord2f(1,1);  glVertex3f(1,1,-1);
		        glTexCoord2f(0,1);  glVertex3f(1,1,1);

		        // top faces
		        glNormal3f(0,1,0);
		        // face v0-v5-v6
		        glTexCoord2f(1,0);  glVertex3f(1,1,1);
		        glTexCoord2f(1,1);  glVertex3f(1,1,-1);
		        glTexCoord2f(0,1);  glVertex3f(-1,1,-1);
		        // face v6-v1-v0
		        glTexCoord2f(0,1);  glVertex3f(-1,1,-1);
		        glTexCoord2f(0,0);  glVertex3f(-1,1,1);
		        glTexCoord2f(1,0);  glVertex3f(1,1,1);

		        // left faces
		        glNormal3f(-1,0,0);
		        // face  v1-v6-v7
		        glTexCoord2f(1,1);  glVertex3f(-1,1,1);
		        glTexCoord2f(0,1);  glVertex3f(-1,1,-1);
		        glTexCoord2f(0,0);  glVertex3f(-1,-1,-1);
		        // face v7-v2-v1
		        glTexCoord2f(0,0);  glVertex3f(-1,-1,-1);
		        glTexCoord2f(1,0);  glVertex3f(-1,-1,1);
		        glTexCoord2f(1,1);  glVertex3f(-1,1,1);

		        // bottom faces
		        glNormal3f(0,-1,0);
		        // face v7-v4-v3
		        glTexCoord2f(0,0);  glVertex3f(-1,-1,-1);
		        glTexCoord2f(1,0);  glVertex3f(1,-1,-1);
		        glTexCoord2f(1,1);  glVertex3f(1,-1,1);
		        // face v3-v2-v7
		        glTexCoord2f(1,1);  glVertex3f(1,-1,1);
		        glTexCoord2f(0,1);  glVertex3f(-1,-1,1);
		        glTexCoord2f(0,0);  glVertex3f(-1,-1,-1);

		        // back faces
		        glNormal3f(0,0,-1);
		        // face v4-v7-v6
		        glTexCoord2f(0,0);  glVertex3f(1,-1,-1);
		        glTexCoord2f(1,0);  glVertex3f(-1,-1,-1);
		        glTexCoord2f(1,1);  glVertex3f(-1,1,-1);
		        // face v6-v5-v4
		        glTexCoord2f(1,1);  glVertex3f(-1,1,-1);
		        glTexCoord2f(0,1);  glVertex3f(1,1,-1);
		        glTexCoord2f(0,0);  glVertex3f(1,-1,-1);
		    glEnd();

		glBindTexture(GL_TEXTURE_2D, 0);
	}


	protected:
		virtual void render( )
		{
			float const angle = 90.0f;

			_checkForGLError( "Render IN ");

			// render teapot to texture
			glViewport( 0, 0, mW, mH );

			glMatrixMode( GL_PROJECTION );

			glLoadIdentity( );

			perspectiveGL( 60.0f, (float)(mW)/mH, 1.0f, 100.0f );

			glMatrixMode( GL_MODELVIEW );

			_checkForGLError( "Render bisect ");


			glLoadIdentity( );

			glTranslatef( 0, 0, -mCameraDistance );

			glBindFramebuffer( GL_FRAMEBUFFER, mFBOID );

			glClearColor( 1, 1, 1, 1 );

			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			glPushMatrix( );

			drawTeapot( );

			glPopMatrix( );

			glBindFramebuffer( GL_FRAMEBUFFER, 0 );

			glBindTexture( GL_TEXTURE_2D, mTextureID );

			glGenerateMipmap( GL_TEXTURE_2D );

			glBindTexture( GL_TEXTURE_2D, 0 );

			_checkForGLError( "End render to texture ");

			// back to normal
			glViewport( 0, 0, mW, mH );

			glMatrixMode( GL_PROJECTION );

			glLoadIdentity( );

			perspectiveGL( 60.0f, (float)(mW)/mH, 1.0f, 100.0f );

			glMatrixMode( GL_MODELVIEW );

			glLoadIdentity( );

			glTranslatef( 0, 0, -mCameraDistance );

			glRotatef( mCameraAngleX, 1, 0, 0 );
			glRotatef( mCameraAngleY, 0, 0, 1 );

			glClearColor( 0, 0, 0, 0 );
			glClear( GL_DEPTH_BUFFER_BIT );

			glBindFramebuffer( GL_READ_FRAMEBUFFER, mFBOID );

			glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

			glBlitFramebuffer( 0, 0, mW, mH, 0, 0, mW, mH, GL_COLOR_BUFFER_BIT, GL_LINEAR );

			glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );

			glPushMatrix( );

			_drawTexturedCube( );

			glPopMatrix( );

			_checkForGLError( "Render OUT "); 
		}

	public:
		FBOTestWindow( )
			: SDLTestWindow( )
			, mFBOID( 0 )
		        , mTextureID( 0 )
	   	        , mRBOID( 0 )
			, mMouseLeftDown( false )
	   		, mMouseRightDown( false)
			, mMouseX( 0 )
		        , mMouseY( 0 )
		        , mCameraAngleX( 45 )
		        , mCameraAngleY( 45 )
		        , mCameraDistance( 6.0f )
	                , mPlayTime( 0 )

		{
			;
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestWindow::initialize( xAttributes );


			// Init GL
			glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );

			//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
			//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

			//glEnable(GL_DEPTH_TEST);
			//glEnable(GL_TEXTURE_2D);
			//glEnable(GL_CULL_FACE);

			glClearColor(0, 0, 0, 0);                   // background color
    			glClearStencil(0);                          // clear stencil buffer
			glClearDepth(1.0f);                         // 0 is near, 1 is far
			glDepthFunc(GL_LEQUAL);


			// create texture object
			glGenTextures(1, &mTextureID);
			glBindTexture(GL_TEXTURE_2D, mTextureID);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);



			//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap generation included in OpenGL v1.4
			_checkForGLError( "Preliminary initialization." );

			glTexImage2D(GL_TEXTURE_2D
				    , 0
				    , GL_RGBA8
				    , mW
				    , mH
			            , 0
				    , GL_RGBA
				    , GL_UNSIGNED_BYTE
				    , 0);

			_checkForGLError( "Create texture object." );

    			glBindTexture(GL_TEXTURE_2D, 0);


			// create fbo and rbo
			glGenFramebuffers(1, &mFBOID);
			glBindFramebuffer(GL_FRAMEBUFFER, mFBOID);

		        // create a renderbuffer object to store depth info
		        // NOTE: A depth renderable image should be attached the FBO for depth test.
		        // If we don't attach a depth renderable image to the FBO, then
		        // the rendering output will be corrupted because of missing depth test.
		        // If you also need stencil test for your rendering, then you must
		        // attach additional image to the stencil attachement point, too.
		        glGenRenderbuffers(1, &mRBOID);
		        glBindRenderbuffer(GL_RENDERBUFFER, mRBOID);
		        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mW, mH);
		        glBindRenderbuffer(GL_RENDERBUFFER, 0);

		        // attach a texture to FBO color attachement point
		        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0);

		        // attach a renderbuffer to depth attachment point
		        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRBOID);

		        //@@ disable color buffer if you don't attach any color buffer image,
		        glBindFramebuffer(GL_FRAMEBUFFER, 0);

			_checkForGLError( "End FBOTestWindow::initialize" );
		}

		virtual void run( )
		{
			while ( !mQuit.isSet( ) )
			{
				render( );
				SDL_GL_SwapWindow( mMainWindow );
				SDL_PumpEvents( );
			}

			_hide( );
		}
};

} // namespace

TEST(sdl_FBO_UT, CreateWindowWithoutExceptions)
{

	FBOTestWindow window;

	window.initialize( { 4, 4, 0, false, false } );

	window.run( );

	BLOCK_EXECUTION;
}
#endif // SDL_FBO_UT
