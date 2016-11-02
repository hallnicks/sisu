#ifndef HELLO_INSTANCING_CC3D95AA2023478E9CA4231F73A8871D_HPP_
#define HELLO_INSTANCING_CC3D95AA2023478E9CA4231F73A8871D_HPP_

#ifdef WIN32
#include <Windows.h>
#elif defined(LINUX)
#include <X11/Xlib.h>
#endif

#include "SDLTest.hpp"
#include "SDLShader.hpp"
#include "Stopwatch.hpp"
#include "Texture2D.hpp"
#include "DevILImage.hpp"
#include "sisumath.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
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

#ifndef ANDROID
#include "Skybox.hpp"
#endif

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

namespace  sisu {

class FrameBufferObject
{
	SDLShader mScreenShader;

	static GLfloat const sQuadVertices[4 * 3 * 2];

	GLuint mScreenQuadVAO
	     , mScreenQuadVBO
	     , mScreenFBO
	     , mScreenRBO
	     , mTextureColorBuffer;

	uint32_t mW, mH;

	// Generates a texture that is suited for attachments to a framebuffer
	GLuint _generateAttachmentTexture(GLboolean depth, GLboolean stencil)
	{
	    	// What enum to use?
	    	GLenum attachment_type;
	    	if(!depth && !stencil)
		{
	        	attachment_type = GL_RGB;
		}
	    	else if(depth && !stencil)
		{
	        	attachment_type = GL_DEPTH_COMPONENT;
		}
	    	else if(!depth && stencil)
		{
	        	//attachment_type = GL_STENCIL_INDEX;
			SISULOG("Stencil buffer is unsupported on OpenGL ES 3.0");
			exit( -1 );
		}

	    	//Generate texture ID and load texture data
	    	GLuint textureID;
	    	glGenTextures(1, &textureID);
	    	glBindTexture(GL_TEXTURE_2D, textureID);

	    	if(!depth && !stencil)
		{
		        glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, mW, mH, 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
		}
		else // Using both a stencil and depth test, needs special format arguments
	        {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, mW, mH, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		}

	    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    	glBindTexture(GL_TEXTURE_2D, 0);

	    return textureID;
	}

	public:
		FrameBufferObject( )
			: mScreenShader( ShaderSourcePair("#version 300 es 						\n"
							  "layout (location = 0) in vec2 position; 			\n"
							  "layout (location = 1) in vec2 texCoords; 			\n"
							  "out vec2 TexCoords; 						\n"
							  "void main() 							\n"
							  "{ 								\n"
							  "    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);  \n"
							  "    TexCoords = texCoords; 					\n"
							  "} 								\n"
							 ,"#version 300 es								   \n"
				    	                  "precision mediump float;				                	   \n"
							  "in vec2 TexCoords; 								   \n"
							  "out vec4 color; 								   \n"
							  "uniform sampler2D screenTexture; 						   \n"
							  "const float offset = 1.0 / 300.0; 						   \n"
							  "void main() 									   \n"
							  "{ 										   \n"
							  "    vec2 offsets[9] = vec2[]( 						   \n"
							  "        vec2(-offset, offset),  						   \n" // top-left
							  "        vec2(0.0f,    offset),  						   \n" // top-center
							  "        vec2(offset,  offset),  						   \n" // top-right
							  "        vec2(-offset, 0.0f),    						   \n" // center-left
							  "        vec2(0.0f,    0.0f),    						   \n" // center-center
							  "        vec2(offset,  0.0f),    						   \n" // center-right
							  "        vec2(-offset, -offset), 						   \n" // bottom-left
							  "        vec2(0.0f,    -offset), 						   \n" // bottom-center
							  "        vec2(offset,  -offset)  						   \n" // bottom-right
							  "    ); 									   \n"
							  "    float kernel[9] = float[](					  	   \n"
							  "        -1.0f, -1.0f, -1.0f, 						   \n"
							  "        -1.0f,  9.0f, -1.0f,							   \n"
							  "        -1.0f, -1.0f, -1.0f 							   \n"
							  "    ); 									   \n"
							  "    vec3 sampleTex[9]; 							   \n"
							  "    for(int i = 0; i < 9; i++) 						   \n"
							  "    { 									   \n"
							  "        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i])); \n"
							  "    } 								           \n"
							  "    vec3 col; 								   \n"
							  "    for(int i = 0; i < 9; i++) 						   \n"
							  "   { 									   \n"
							  "        col += sampleTex[i] * kernel[i]; 					   \n"
							  "   } 									   \n"
							  "    color = vec4(col, 1.0);							   \n"
							  "} 										   \n" ) )
			, mScreenQuadVAO( 0 )
	 	        , mScreenQuadVBO( 0 )
	 	        , mScreenFBO( 0 )
	    	        , mScreenRBO( 0 )
			, mTextureColorBuffer( 0 )
			, mW( 0 )
			, mH( 0 )

		{
			;
		}

		void initialize( uint32_t const xW, uint32_t const xH )
		{
			mW = xW;
			mH = xH;

			mScreenShader.initialize( );

			// VAO, VBO
			glGenVertexArrays(1, &mScreenQuadVAO);
			glGenBuffers(1, &mScreenQuadVBO);
			glBindVertexArray( mScreenQuadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, mScreenQuadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(sQuadVertices), &sQuadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
			glBindVertexArray(0);

			glGenFramebuffers(1, &mScreenFBO);
			glBindFramebuffer(GL_FRAMEBUFFER, mScreenFBO);
  		        // Create a color attachment texture
		       	mTextureColorBuffer = _generateAttachmentTexture( false, false );
		       	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureColorBuffer, 0);
		       	// Create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
			glGenRenderbuffers(1, &mScreenRBO);
			glBindRenderbuffer(GL_RENDERBUFFER, mScreenRBO );
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mW, mH ); // Use a single renderbuffer object for both a depth$
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mScreenRBO ); // Now actually attach it
		        // Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
			        SISULOG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
				exit(-1 );
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void operator()( std::function<void(void)> xLambda )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, mScreenFBO );
			xLambda( );
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		}

		void render( )
		{
        		// Clear all relevant buffers
			glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Set clear color to white (not really necessery actually, since we won't be able to se$
        		glClear(GL_COLOR_BUFFER_BIT);
        		glDisable(GL_DEPTH_TEST); // We don't care about depth information when rendering a single quad

        		// Draw Screen
			mScreenShader([&]( )
			{
			        glBindVertexArray( mScreenQuadVAO );
			        glBindTexture(GL_TEXTURE_2D, mTextureColorBuffer);       // Use the color attachment texture as the texture of the quad plane
			        glDrawArrays(GL_TRIANGLES, 0, 6);
			        glBindVertexArray(0);
			});
		}
};

GLfloat const FrameBufferObject::sQuadVertices[4 * 3 * 2] = {   // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // Positions   // TexCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
};

class HelloInstancing : public SDLTestWindow
{
	Mouse mMouse;
	Keyboard mKeyboard;

	Overlay2D mOverlay2D;

	CubeRenderer mCubeRenderer;

	TextRenderer mTextRenderer;

#ifndef ANDROID
	const char * mSkyboxFaces[ 6 ] = { "resources/testinput/right.png"
					 , "resources/testinput/left.png"
					 , "resources/testinput/top.png"
					 , "resources/testinput/bottom.png"
					 , "resources/testinput/back.png"
					 , "resources/testinput/front.png" };
	Skybox mSkybox;
#endif

	CursorRenderer mCursorRenderer;
	MouseEventInfo mCursorPosition;

	Camera mCamera;

	Texture2D mTexture;
	DevILImage mPNGImage;

	Oscillator<int32_t> mOscW, mOscH;

	Oscillator<GLfloat> mOscR, mOscAlpha;

	SDLShader mModelShader;

	Model mNanosuitGuy;

	FrameBufferObject mFBO;

	void _initialize2DOverlay( )
	{
		mOverlay2D.initialize( mW, mH );

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
		        model = glm::translate(model, glm::vec3(0.0f, 0.0, 0.0f));
		        model = glm::scale(model, glm::vec3(0.12f, 0.12f, 0.12f));
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

#ifndef ANDROID
			mSkybox.render( mCamera.getViewMatrix( ), mCamera.getFOV( ) );
#endif

			//mCubeRenderer.render3DScene( );

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
					        , glm::vec2( mW / 4, mH / 3 ) );


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
#ifndef ANDROID
			, mSkybox( mSkyboxFaces )
#endif
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
			SISULOG("In HelloInstancing::Initialize");
			SDLTestWindow::initialize( xAttributes );

			SISULOG( "Initialize shader." );
			mModelShader.initialize( );

			SISULOG("Initialize FBO" );
			mFBO.initialize( mW, mH );

			//SISULOG( "Initialize model" );
			mNanosuitGuy.initialize( "resources/objects/nanosuit/nanosuit.obj" );
			//mNanosuitGuy.initialize( "resources/objects/Gadv.obj" );
			//mNanosuitGuy.initialize( "resources/objects/lowpolyman.obj" );

			SISULOG("Initialize 2D Overlay." );
			_initialize2DOverlay( );

			SISULOG( "Initialize cube renderer." );
			mCubeRenderer.initialize( mW, mH, &mCamera );

			SISULOG ("Initialize text renderer." );
			mTextRenderer.initialize( mW, mH );

			static uint32_t const sCursorWidth  = 32;
			static uint32_t const sCursorHeight = 32;

			// TODO: Disable for android..
			SISULOG( "Initialize cursor renderer. " );
			mCursorRenderer.initialize( sCursorWidth
						  , sCursorHeight
						  , "resources/testinput/cursor.png" );

#ifndef ANDROID
			SISULOG( "Initialize skybox." );
			mSkybox.initialize( mW, mH );
#endif

			SISULOG( "Initialize oscillator" );
			mOscW.setMinMax( 0, mW );
			mOscH.setMinMax( 0, mH );

			SISULOG( "Initialize mouse. " );
			mMouse.registerCallback([&](MouseEventInfo const & xEvent){

				mCursorPosition = xEvent;

				mCubeRenderer.onMouseEvent( xEvent );
			});

			mMouse.listen( );

			SISULOG( "Initialize keyboard." );
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

			SISULOG("Beginning main render loop." );
			do
			{
				t.startMs( );

				mCubeRenderer.updateCameras( );

				mCursorRenderer.hideExternalCursor( );

				mFBO([&]( )
				{
					render( );

					// TODO: Ideally, we should be able to compute the text size based on the font
					// for a given string and use that as our offset instead of 225 x 150.
					mTextRenderer.drawString( &mOverlay2D
								, ss.str( ).c_str( )
								, glm::vec2( mW - 225, mH - 150 ) );

				});

				mFBO.render( );

				ss.str("");

				_checkForGLError( );

				SDL_PumpEvents( );

				glFlush( );

				SDL_GL_SwapWindow( mMainWindow );


				if ( ( accum += t.stop( ) ) >= 60000.0 )
				{
					SISULOG("HelloInstancing test time has elapsed.");
					break;
				}

				++frames;

				fps = accum / frames;

				ss << "FPS: " << fps;


			} while ( 1 );

			SISULOG("Exiting main render loop.");
			mMouse.stopListening( );
			mKeyboard.stopListening( );

			_hide( );
		}
};

} // namespace sisu

#endif //HELLO_INSTANCING_CC3D95AA2023478E9CA4231F73A8871D_HPP_
