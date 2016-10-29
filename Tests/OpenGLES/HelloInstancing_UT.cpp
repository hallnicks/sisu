#ifdef OPENGLES_HELLOINSTANCING_UT
#ifdef OPENGLES

// TODO: Generally, we need to delete VAOs and VBos
#include "test.hpp"

#ifdef WIN32
#include <Windows.h>
#elif defined(LINUX)
#include <X11/Xlib.h>
#endif

#include "SDLTest.hpp"
#include "SDLShader.hpp"
#include "Stopwatch.hpp"
#include "Texture2D.hpp"
#include "PNGImage.hpp"
#include "sisumath.hpp"
#include "mouse.hpp"
#include "keyboard.hpp"
#include "GLCharacterMap.hpp"
#include "Camera.hpp"
#include "CubeRenderer.hpp"
#include "TextRenderer.hpp"
#include "CursorRenderer.hpp"
#include "DevILImage.hpp"
#include "Model.hpp"
#include "Overlay2D.hpp"
#include "Oscillator.hpp"
#include "Quad.hpp"
#include "Skybox.hpp"
#include "Sprite.hpp"


#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <iostream>
#include <string>
#include <sstream>

using namespace sisu;

namespace {

class HelloInstancing_UT : public context
{
	public:
		HelloInstancing_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

class HelloInstancing : public SDLTestWindow
{
	Mouse mMouse;
	Keyboard mKeyboard;

	Overlay2D mOverlay2D;

	CubeRenderer mCubeRenderer;

	TextRenderer mTextRenderer;

	const char * mSkyboxFaces[ 6 ] = { "resources/testinput/right.png"
					 , "resources/testinput/left.png"
					 , "resources/testinput/top.png"
					 , "resources/testinput/bottom.png"
					 , "resources/testinput/back.png"
					 , "resources/testinput/front.png" };
	Skybox mSkybox;

	CursorRenderer mCursorRenderer;
	MouseEventInfo mCursorPosition;

	Camera mCamera;

	Texture2D mTexture;
	PNGImage mPNGImage;

	Oscillator<int32_t> mOscW, mOscH;

	Oscillator<GLfloat> mOscR, mOscAlpha;

	SDLShader mModelShader;

	Model mNanosuitGuy;

	void _initialize2DOverlay( )
	{
		mOverlay2D.initialize( mW, mH );

		if ( !mPNGImage.getIsValid( ) )
		{
			std::cerr << "Unit test resource testinput01.png not found or is corrupt." << std::endl;
			exit( -1 );
		}

		mTexture.initialize( mPNGImage.getWidth( )
				   , mPNGImage.getHeight( )
				   , mPNGImage.toGLTextureBuffer( ) );
	}

	void _renderNanosuitGuy( )
	{
		mModelShader([&]( ) {
		        glm::mat4 projection = glm::perspective(mCamera.getFOV( ), (float)mW/(float)mH, 0.1f, 100.0f);
		        mModelShader.getUniforms().setUniformMatrix4fv("projection", projection);
		        mModelShader.getUniforms().setUniformMatrix4fv("view", mCamera.getViewMatrix( ) );

		        // Draw the loaded model
			glm::mat4 model;
		        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		        model = glm::scale(model, glm::vec3(10.2f, 10.2f, 10.2f)); // It's a bit too big for our scene, so scale it down
		        mModelShader.getUniforms().setUniformMatrix4fv("model", model);
			mNanosuitGuy.render( mModelShader );
		} );

	}

	protected:
		virtual void render( )
		{
			// Set the viewport
			glViewport( 0, 0, mW, mH );

			glEnable( GL_DEPTH_TEST );

			// Clear the color buffer
			glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

			mSkybox.render( mCamera.getViewMatrix( ), mCamera.getFOV( ) );

			mCubeRenderer.render3DScene( );

			_renderNanosuitGuy( );

			glDepthMask( GL_FALSE );

			glActiveTexture( GL_TEXTURE5 );

			mOverlay2D.drawTexture( mTexture
		 			      , glm::vec2( 0, 0 )
 	  			 	      , glm::vec2( ++mOscW, ++mOscH )
					      , ++mOscR
			    		      , glm::vec4( dRand( 0.0f, 1.0f )
					       		 , dRand( 0.0f, 1.0f )
	                                       		 , dRand( 0.0f, 1.0f )
					       		 , ++mOscAlpha ) );

			std::stringstream versionString;

			versionString << "Vendor:       " << glGetString( GL_VENDOR                   ) << std::endl
				      << "Renderer:     " << glGetString( GL_RENDERER                 ) << std::endl
				      << "Version:      " << glGetString( GL_VERSION                  ) << std::endl
				      << "GLSL Version: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;

			mTextRenderer.drawString( &mOverlay2D
						, versionString.str( ).c_str( )
					        , glm::vec2( mW / 3, mH / 2 ) );


			mCursorRenderer.drawCursor( &mOverlay2D
						  , mCursorPosition.x
						  , mCursorPosition.y );

			glDepthMask( GL_TRUE );

			_checkForGLError( "After render" );
		}

	public:
		HelloInstancing( )
			: SDLTestWindow( )
			, mMouse( )
			, mTexture( )
			, mOverlay2D( )
			, mCubeRenderer( )
			, mCursorPosition( )
			, mCamera( )
			, mTextRenderer( )
			, mSkybox( mSkyboxFaces )
			, mPNGImage( "resources/testinput/testinput16.png" )
			, mOscW( 0, 0, 0, 25 )
			, mOscH( 0, 0, 0, 25 )
			, mOscR( 0.0f, 0.0f, 360.0f, 0.10f )
			, mOscAlpha( 1.0f, 0.0f, 1.0f, 0.05f )
			, mModelShader( ShaderSourcePair( "#version 300 es                                                      \n"
							  "layout (location = 0) in vec3 position;				\n"
							  "layout (location = 1) in vec3 normal;				\n"
							  "layout (location = 2) in vec2 texCoords;				\n"
							  "out vec2 TexCoords;							\n"
							  "uniform mat4 model;							\n"
							  "uniform mat4 view;							\n"
							  "uniform mat4 projection;						\n"
							  "void main()								\n"
							  "{									\n"
							  "    gl_Position = projection * view * model * vec4(position, 1.0f);	\n"
							  "    TexCoords = texCoords;						\n"
							  "}									\n"
							 ,"#version 300 es                                                      \n"
				    	                  "precision mediump float;				                \n"
							  "in vec2 TexCoords;							\n"
							  "out vec4 color;							\n"
							  "uniform sampler2D texture_diffuse1;					\n"
							  "uniform sampler2D texture_specular1;					\n"
							  "uniform sampler2D texture_normal1;					\n"
							  "uniform sampler2D texture_height1;					\n"
							  "void main()								\n"
							  "{									\n"
							  "    color = vec4(texture(texture_diffuse1, TexCoords));		\n"
							  "}									\n" ) )
		{
			;
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestWindow::initialize( xAttributes );

			mModelShader.initialize( );

			mNanosuitGuy.initialize( "resources/objects/nanosuit/nanosuit.obj" );
			//mNanosuitGuy.initialize( "resources/objects/nifskope.obj" );
			//mNanosuitGuy.initialize( "resources/diloph/Full.obj" );
			//mNanosuitGuy.initialize( "resources/M1911.obj" );
			_initialize2DOverlay( );

			mCubeRenderer.initialize( mW, mH, &mCamera );
			mTextRenderer.initialize( mW, mH );

			static uint32_t const sCursorWidth  = 32;
			static uint32_t const sCursorHeight = 32;

			mCursorRenderer.initialize( sCursorWidth
						  , sCursorHeight
						  , "resources/testinput/cursor.png" );

			mSkybox.initialize( mW, mH );

			mOscW.setMinMax( 0, mW );
			mOscH.setMinMax( 0, mH );

			mMouse.registerCallback([&](MouseEventInfo const & xEvent){

				mCursorPosition = xEvent;

				mCubeRenderer.onMouseEvent( xEvent );
			});

			mMouse.listen( );

			mKeyboard.registerCallback( [&]( KeyboardEvent const & xEvent ){
				mCubeRenderer.onKeyboardEvent( xEvent );
			});

			mKeyboard.listen( );
		}

		virtual void run( )
		{
			Stopwatch t;

			double accum = 0.0
			     , fps   = 0.0;

			uint32_t frames = 0;

			std::stringstream ss;

			do
			{
				t.startMs( );

				mCubeRenderer.updateCameras( );

				mCursorRenderer.hideExternalCursor( );

				render( );

				// TODO: Ideally, we should be able to compute the text size based on the font
				// for a given string and use that as our offset.
				mTextRenderer.drawString( &mOverlay2D
							, ss.str( ).c_str( )
							, glm::vec2( mW - 225, mH - 150 ) );

				ss.str("");

				_checkForGLError( );

				SDL_PumpEvents( );

				glFlush( );

				SDL_GL_SwapWindow( mMainWindow );


				if ( ( accum += t.stop( ) ) >= 600000.0 )
				{
					break;
				}

				++frames;

				fps = accum / frames;

				ss << "FPS: " << fps;


			} while ( 1 );

			mMouse.stopListening( );
			mKeyboard.stopListening( );

			_hide( );
		}
};

} // namespace

TEST(HelloInstancing_UT, HelloInstancing)
{
	HelloInstancing * window = new HelloInstancing;
	window->initialize( { 3, 0, 1, false, false } );
	window->run( );
	delete window;
}
#endif // OPENGLES
#endif // OPENGLES_HELLOINSTANCING_UT

